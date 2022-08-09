#include <sstream>
#include <numeric>

#include <SysY.hpp>
#include <stlpro.hpp>

#include "frontend/frontendHeader.hpp"


//  conversion between literal and btype
using namespace sup;

namespace frontend {
int shapeToStride(const std::vector<int> & shape) {
	return std::accumulate(shape.begin(), shape.end(), 1, std::multiplies<>());
}

IdxView::IdxView(std::vector<int> _shape)
	: idx(_shape.size(), 0), shape(std::move(_shape)) {
}

void IdxView::addOnDimN(int n, int a) {
	if (n < 0) {
		n = int(shape.size()) + n;
	}
	for (int i = n; i >= 0; --i) {
		idx[i] += a;
		if (idx[i] >= shape[i]) {
			a = idx[i] / shape[i];
			idx[i] = idx[i] % shape[i];
		} else {
			break;
		}
	}
}

void IdxView::set0AfterNDim(int n) {
	if (n < 0) {
		n = int(shape.size()) + n;
	}
	for (int i = n + 1; i < int(idx.size()); ++i) {
		idx[i] = 0;
	}
}

void IdxView::set0AfterNDimAndCarry(int n) {
	for (int i = n + 1; i < int(idx.size()); ++i) {
		idx[i] = 0;
	}
	addOnDimN(n);
}

int IdxView::getStride() const {
	return std::accumulate(shape.begin(), shape.end(), 1, std::multiplies<>());
}

int IdxView::getPos() const {
	return sup::idxToPos(idx, shape);
}

bool IdxView::isAll0AfterNDim(int n) {
	for (int i = n + 1; i < int(idx.size()); ++i) {
		if (idx[i] != 0) {
			return false;
		}
	}
	return true;
}

std::list<ircode::IRInstr *> fromArrayItemsToInstrs(
	ircode::IRModule & ir,
	std::set<ArrayItem<ircode::AddrOperand *>> && items,
	const std::vector<int> & shape,
	ircode::AddrVariable * varMemBaseAddr,
	const TypeInfo & typeOfElement
) {
	if (shape.empty()) {
		com::Assert(
			items.size() == 1,
			"Items should have only one item when defining scalar var.", CODEPOS
		);
		auto item = *items.begin();
		items.clear();
		auto instrs = std::move(item.instrsToInit);
		auto [pConversionAddr, _, convertInstrs] = genAddrConversion(
			ir, item.val, typeOfElement
		);
		instrs.splice(instrs.end(), std::move(convertInstrs));
		instrs.emplace_back(
			ir.instrPool.emplace_back(
				ircode::InstrStore(pConversionAddr, varMemBaseAddr)
			)
		);
		return instrs;
	}
	auto instrsRes = std::list<ircode::IRInstr *>();
	auto pBegin = ir.addrPool.emplace_back(
		ircode::AddrVariable(
			PointerType(typeOfElement)
		)
	);
	auto idx0 = [&shape, &ir]() -> std::vector<ircode::AddrOperand *> {
		auto idx0 = std::vector<ircode::AddrOperand *>();
		for (int sz = int(shape.size()) + 1; sz; --sz) {
			idx0.emplace_back(
				ir.addrPool.emplace_back(
					ircode::AddrStaticValue(IntType(), IntStaticValue(0))
				)
			);
		}
		return idx0;
	}();
	instrsRes.emplace_back(
		ir.instrPool.emplace_back(
			ircode::InstrGetelementptr(pBegin, varMemBaseAddr, std::move(idx0))
		)
	);
	for (auto & item: items) {
		auto instrsToInit = item.instrsToInit;
		instrsRes.splice(instrsRes.end(), std::move(instrsToInit));
		//  type conversion
		auto [pValAddr, type, conversionInstrs] =
			genAddrConversion(ir, item.val, typeOfElement);
		instrsRes.splice(instrsRes.end(), std::move(conversionInstrs));
		auto pValMem = ir.addrPool.emplace_back(
			ircode::AddrVariable(PointerType(typeOfElement))
		);
		auto bias = static_cast<ircode::AddrOperand *>(ir.addrPool.emplace_back(
			ircode::AddrStaticValue(
				IntType(), IntStaticValue(item.getPos(shape))
			)
		));
		instrsRes.emplace_back(
			ir.instrPool.emplace_back(
				ircode::InstrGetelementptr(pValMem, pBegin, {bias})
			)
		);
		instrsRes.emplace_back(
			ir.instrPool.emplace_back(
				ircode::InstrStore(pValAddr, pValMem)
			)
		);
	}
	com::addRuntimeWarning(
		"Initialization of array can be optimized on frontend.", CODEPOS, true
	);
	items.clear();
	return instrsRes;
}

std::unique_ptr<sup::StaticValue> fromArrayItemsToStaticValue(
	ircode::IRModule & ir,
	const std::set<ArrayItem<std::unique_ptr<StaticValue>>> & items,
	const std::vector<int> & shape,
	const TypeInfo & typeOfElement
) {
	if (shape.empty()) {
		com::Assert(
			items.size() == 1, "There should be only one element when defining var.",
			CODEPOS
		);
		return convertOnSV(*items.begin()->val, typeOfElement);
	}
	std::vector<std::unique_ptr<StaticValue>> staticValueArray;
	switch (typeOfElement.type) {
		case Type::Int_t: {
			auto res = std::make_unique<IntArrayStaticValue>(shape);
			for (auto & item: items) {
				auto * pVal = com::dynamic_cast_uPtr_get<IntStaticValue>(item.val);
				if (pVal->value) {
					res->insertValue(item.idx, *pVal);
				}
			}
			return res;
		}
		case Type::Float_t: {
			auto res = std::make_unique<FloatArrayStaticValue>(shape);
			for (auto & item: items) {
				auto * pVal = com::dynamic_cast_uPtr_get<FloatStaticValue>(item.val);
				if (bool(pVal->value)) {
					res->insertValue(item.idx, *pVal);
				}
			}
			return res;
		}
		case Type::Bool_t: {
			com::Throw("bool not supported.", CODEPOS);
		}
		default: com::Throw("type should be one of int, float, bool.", CODEPOS);
	}
}


}