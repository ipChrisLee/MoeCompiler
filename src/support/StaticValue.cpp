#include "StaticValue.hpp"

#include <numeric>

#include <stlpro.hpp>

#include "support/TypeInfo.hpp"


namespace sup {
std::unique_ptr<StaticValue>
calcOfFloat(float fl, float fr, const std::string & op) {
	std::unique_ptr<StaticValue> upSV;
	com::regSwitch(
		op, {
			{
				"\\*",    [&upSV, fl, fr]() {
				upSV = std::make_unique<FloatStaticValue>(fl * fr);
			}},
			{
				"\\-",    [&upSV, fl, fr]() {
				upSV = std::make_unique<FloatStaticValue>(fl - fr);
			}},
			{
				"\\+",    [&upSV, fl, fr]() {
				upSV = std::make_unique<FloatStaticValue>(fl + fr);
			}},
			{
				"\\/",    [&upSV, fl, fr]() {
				upSV = std::make_unique<FloatStaticValue>(fl / fr);
			}},
			{
				"%",      []() {
				com::Throw("`float % float` is illegal!", CODEPOS);
			}},
			{
				"&&",     [&upSV, fl, fr]() {
				upSV = std::make_unique<BoolStaticValue>(fl && fr);//NOLINT
			}},
			{
				"\\|\\|", [&upSV, fl, fr]() {
				upSV = std::make_unique<BoolStaticValue>(fl || fr);//NOLINT
			}},
			{
				"<",      [&upSV, fl, fr]() {
				upSV = std::make_unique<BoolStaticValue>(fl < fr);
			}},
			{
				">",      [&upSV, fl, fr]() {
				upSV = std::make_unique<BoolStaticValue>(fl > fr);
			}},
			{
				"<=",     [&upSV, fl, fr]() {
				upSV = std::make_unique<BoolStaticValue>(fl <= fr);
			}},
			{
				">=",     [&upSV, fl, fr]() {
				upSV = std::make_unique<BoolStaticValue>(fl >= fr);
			}},
			{
				"==",     [&upSV, fl, fr]() {
				upSV = std::make_unique<BoolStaticValue>(fl == fr);
			}},
			{
				"!=",     [&upSV, fl, fr]() {
				upSV = std::make_unique<BoolStaticValue>(fl != fr);
			}},
			{
				"!",      [&upSV, fr]() {
				upSV = std::make_unique<BoolStaticValue>(!fr);//NOLINT
			}},
		}
	);
	return upSV;
}

std::unique_ptr<StaticValue>
calcOfInt(int il, int ir, const std::string & op) {
	std::unique_ptr<StaticValue> upSV;
	com::regSwitch(
		op, {
			{
				"\\*",    [&upSV, il, ir]() {
				upSV = std::make_unique<IntStaticValue>(il * ir);
			}},
			{
				"\\-",    [&upSV, il, ir]() {
				upSV = std::make_unique<IntStaticValue>(il - ir);
			}},
			{
				"\\+",    [&upSV, il, ir]() {
				upSV = std::make_unique<IntStaticValue>(il + ir);
			}},
			{
				"\\/",    [&upSV, il, ir]() {
				upSV = std::make_unique<IntStaticValue>(il / ir);
			}},
			{
				"%",      [&upSV, il, ir]() {
				upSV = std::make_unique<IntStaticValue>(il % ir);
			}},
			{
				"&&",     [&upSV, il, ir]() {
				upSV = std::make_unique<BoolStaticValue>(il && ir);
			}},
			{
				"\\|\\|", [&upSV, il, ir]() {
				upSV = std::make_unique<BoolStaticValue>(il || ir);
			}},
			{
				"<",      [&upSV, il, ir]() {
				upSV = std::make_unique<BoolStaticValue>(il < ir);
			}},
			{
				">",      [&upSV, il, ir]() {
				upSV = std::make_unique<BoolStaticValue>(il > ir);
			}},
			{
				"<=",     [&upSV, il, ir]() {
				upSV = std::make_unique<BoolStaticValue>(il <= ir);
			}},
			{
				">=",     [&upSV, il, ir]() {
				upSV = std::make_unique<BoolStaticValue>(il >= ir);
			}},
			{
				"==",     [&upSV, il, ir]() {
				upSV = std::make_unique<BoolStaticValue>(il == ir);
			}},
			{
				"!=",     [&upSV, il, ir]() {
				upSV = std::make_unique<BoolStaticValue>(il != ir);
			}},
			{
				"!",      [&upSV, ir]() {
				upSV = std::make_unique<BoolStaticValue>(!ir);
			}},
		}
	);
	return upSV;
}

std::unique_ptr<StaticValue>
calcOfBool(bool bl, bool br, const std::string & op) {
	std::unique_ptr<StaticValue> upSV;
	com::regSwitch(
		op, {
			{
				"\\*",    [&upSV, bl, br]() {
				upSV = std::make_unique<IntStaticValue>(bl * br);
			}},
			{
				"\\-",    [&upSV, bl, br]() {
				upSV = std::make_unique<IntStaticValue>(bl - br);
			}},
			{
				"\\+",    [&upSV, bl, br]() {
				upSV = std::make_unique<IntStaticValue>(bl + br);
			}},
			{
				"\\/",    [&upSV, bl, br]() {
				upSV = std::make_unique<IntStaticValue>(bl / br);
			}},
			{
				"%",      [&upSV, bl, br]() {
				upSV = std::make_unique<IntStaticValue>(bl % br);
			}},
			{
				"&&",     [&upSV, bl, br]() {
				upSV = std::make_unique<BoolStaticValue>(bl && br);
			}},
			{
				"\\|\\|", [&upSV, bl, br]() {
				upSV = std::make_unique<BoolStaticValue>(bl || br);
			}},
			{
				"<",      [&upSV, bl, br]() {
				upSV = std::make_unique<BoolStaticValue>(bl < br);
			}},
			{
				">",      [&upSV, bl, br]() {
				upSV = std::make_unique<BoolStaticValue>(bl > br);
			}},
			{
				"<=",     [&upSV, bl, br]() {
				upSV = std::make_unique<BoolStaticValue>(bl <= br);
			}},
			{
				">=",     [&upSV, bl, br]() {
				upSV = std::make_unique<BoolStaticValue>(bl >= br);
			}},
			{
				"==",     [&upSV, bl, br]() {
				upSV = std::make_unique<BoolStaticValue>(bl == br);
			}},
			{
				"!=",     [&upSV, bl, br]() {
				upSV = std::make_unique<BoolStaticValue>(bl != br);
			}},
			{
				"!",      [&upSV, br]() {
				upSV = std::make_unique<BoolStaticValue>(!br);
			}},
		}
	);
	return upSV;
}

std::unique_ptr<StaticValue>
zeroExtensionValueOfType(const TypeInfo & typeInfo) {
	switch (typeInfo.type) {
		case Type::Bool_t: {
			return std::make_unique<BoolStaticValue>();
		}
		case Type::Int_t: {
			return std::make_unique<IntStaticValue>();
		}
		case Type::Float_t: {
			return std::make_unique<FloatStaticValue>();
		}
		case Type::IntArray_t: {
			return std::make_unique<IntArrayStaticValue>(
				dynamic_cast<const IntArrayType &>(typeInfo).shape
			);
		}
		case Type::FloatArray_t: {
			return std::make_unique<FloatArrayStaticValue>(
				dynamic_cast<const FloatArrayType &>(typeInfo).shape
			);
		}
		default: {
			com::Throw("", CODEPOS);
		}
	}
}

StaticValue::StaticValue(const TypeInfo & typeInfo)
	: uPtrInfo(com::dynamic_cast_uPtr<TypeInfo>(typeInfo.cloneToUniquePtr())) {
}

StaticValue::StaticValue(const StaticValue & staticValue)
	: uPtrInfo(
	com::dynamic_cast_uPtr<TypeInfo>(staticValue.uPtrInfo->cloneToUniquePtr())
) {
}

std::unique_ptr<StaticValue>
StaticValue::calc(
	const std::string & op, const StaticValue &
) const {
	com::Throw("This method should not be invoked.", CODEPOS);
}

std::unique_ptr<StaticValue>
StaticValue::calc(const std::string & op) const {
	com::Throw("This method should not be invoked.", CODEPOS);
}

void StaticValue::insertValue(const VI & idx, StaticValue & staticValue) {
	com::Throw("This override should not be called.", CODEPOS);
}

std::unique_ptr<moeconcept::Cloneable>
FloatStaticValue::_cloneToUniquePtr() const {
	return std::make_unique<FloatStaticValue>(*this);
}

FloatStaticValue::FloatStaticValue(const std::string & literal)
	: StaticValue(FloatType()), value(literalToFloat(literal)) {
}

std::string FloatStaticValue::toLLVMIR() const {
	return floatToString(value);
}

std::unique_ptr<StaticValue>
FloatStaticValue::getValue(const VI &) const {
	return com::dynamic_cast_uPtr<StaticValue>(cloneToUniquePtr());
}

std::unique_ptr<StaticValue> FloatStaticValue::calc(
	const std::string & op, const StaticValue & oVal
) const {
	std::unique_ptr<StaticValue> upSVRes;
	switch (oVal.getType().type) {
		case Type::Float_t: {
			upSVRes = calcOfFloat(
				value,
				dynamic_cast<const FloatStaticValue &>(oVal).value, op
			);
			break;
		}
		case Type::Int_t: {
			upSVRes = calcOfFloat(
				value,
				float(dynamic_cast<const IntStaticValue &>(oVal).value), op
			);
			break;
		}
		case Type::Bool_t: {
			upSVRes = calcOfFloat(
				value,
				float(dynamic_cast<const BoolStaticValue &>(oVal).value),
				op
			);
			break;
		}
		default: {
			com::Throw("Type of oVal should be Int_t/Float_t/Bool_t.", CODEPOS);
		}
	}
	return upSVRes;
}

FloatStaticValue::FloatStaticValue(float value)
	: StaticValue(FloatType()), value(value) {
}

std::unique_ptr<StaticValue>
FloatStaticValue::calc(const std::string & op) const {
	return calcOfFloat(0, value, op);
}

std::unique_ptr<moeconcept::Cutable> FloatStaticValue::_cutToUniquePtr() {
	return std::make_unique<FloatStaticValue>(std::move(*this));
}

void FloatStaticValue::insertValue(const VI & idx, StaticValue & staticValue) {
	auto pSV = com::dynamic_cast_uPtr<FloatStaticValue>(convertOnSV(staticValue, FloatType()));
	value = pSV->value;
}

std::unique_ptr<moeconcept::Cloneable>
IntStaticValue::_cloneToUniquePtr() const {
	return std::make_unique<IntStaticValue>(*this);
}

IntStaticValue::IntStaticValue(const std::string & literal)
	: StaticValue(IntType()), value(literalToInt(literal)) {
}

std::string IntStaticValue::toLLVMIR() const {
	return intToString(value);
}

std::unique_ptr<StaticValue>
IntStaticValue::getValue(const VI &) const {
	return com::dynamic_cast_uPtr<StaticValue>(cloneToUniquePtr());
}

IntStaticValue::IntStaticValue(int value)
	: StaticValue(IntType()), value(value) {
}

std::unique_ptr<StaticValue> IntStaticValue::calc(
	const std::string & op, const StaticValue & oVal
) const {
	std::unique_ptr<StaticValue> upSVRes;
	switch (oVal.getType().type) {
		case Type::Float_t: {
			upSVRes = calcOfFloat(
				float(value),
				dynamic_cast<const FloatStaticValue &>(oVal).value, op
			);
			break;
		}
		case Type::Int_t: {
			upSVRes = calcOfInt(
				value,
				dynamic_cast<const IntStaticValue &>(oVal).value, op
			);
			break;
		}
		case Type::Bool_t: {
			upSVRes = calcOfInt(
				value,
				dynamic_cast<const BoolStaticValue &>(oVal).value, op
			);
			break;
		}
		default: {
			com::Throw("Type of oVal should be Int_t/Float_t/Bool_t.", CODEPOS);
		}
	}
	return upSVRes;
}

std::unique_ptr<StaticValue>
IntStaticValue::calc(const std::string & op) const {
	return calcOfInt(0, value, op);
}

std::unique_ptr<moeconcept::Cutable> IntStaticValue::_cutToUniquePtr() {
	return std::make_unique<IntStaticValue>(std::move(*this));
}

void IntStaticValue::insertValue(const VI & idx, StaticValue & staticValue) {
	auto pSV = com::dynamic_cast_uPtr<IntStaticValue>(convertOnSV(staticValue, IntType()));
	value = pSV->value;
}

std::unique_ptr<moeconcept::Cloneable>
FloatArrayStaticValue::_cloneToUniquePtr() const {
	return std::make_unique<FloatArrayStaticValue>(*this);
}

std::string FloatArrayStaticValue::toLLVMIR() const {
	auto floatStaticValue = FloatStaticValue(0);
	std::function<std::string(int, int, int)> fun = [&, this](
		int from, int to, int dim
	) -> std::string {
		if (dim == int(shape.size())) {
			std::string buf;
			for (int i = from; i < to; ++i) {
				auto idx = posToIdx(i, shape);
				auto pVal = value.find(idx);
				if (pVal == value.end()) {
					buf += floatStaticValue.getType().toLLVMIR() + " " +
						floatStaticValue.toLLVMIR() + ", ";
				} else {
					buf += pVal->second.getType().toLLVMIR() + " " +
						pVal->second.toLLVMIR() + ", ";
				}
			}
			buf.pop_back();
			buf.pop_back();
			return buf;
		}
		std::vector<int> nowShape;
		for (int i = dim; i < int(shape.size()); ++i) {
			nowShape.push_back(shape[i]);
		}
		auto buf = std::string();
		if (dim) {
			FloatArrayType fat(nowShape);
			buf += fat.toLLVMIR();
		}
		buf += "[";
		int sz = 1;
		for (int i = dim + 1; i < int(shape.size()); ++i) { sz *= shape[i]; }
		for (int i = 0; i < shape[dim]; ++i
			) {
			buf += fun(from + sz * i, from + sz * (i + 1), dim + 1) + ", ";
		}
		buf.pop_back();
		buf.pop_back();
		buf += "]";
		return buf;
	};
	int sz = 1;
	for (int x: shape) { sz *= x; }
	return fun(0, sz, 0);
}

[[nodiscard]] std::unique_ptr<StaticValue>
FloatArrayStaticValue::getValue(const VI & ind) const {
	if (ind.size() == shape.size()) {
		auto p = value.find(ind);
		if (p == value.end()) {
			return zero.getValue({ });
		} else {
			return p->second.getValue({ });
		}
	} else {
		com::TODO("Maybe do not need this.", CODEPOS);
	}
}

FloatArrayStaticValue::FloatArrayStaticValue(std::vector<int> _shape) :
	StaticValue(sup::FloatArrayType(_shape)), shape(std::move(_shape)) {
}

void FloatArrayStaticValue::insertValue(const VI & idx, StaticValue & staticValue) {
	auto pSV = com::dynamic_cast_uPtr<FloatStaticValue>(convertOnSV(staticValue, FloatType()));
	value.emplace(idx, std::move(*pSV));
}

std::unique_ptr<moeconcept::Cutable>
FloatArrayStaticValue::_cutToUniquePtr() {
	return std::make_unique<FloatArrayStaticValue>(std::move(*this));
}

std::unique_ptr<moeconcept::Cloneable>
IntArrayStaticValue::_cloneToUniquePtr() const {
	return std::make_unique<IntArrayStaticValue>(*this);
}

std::string IntArrayStaticValue::toLLVMIR() const {
	std::function<std::string(int, int, int)> fun = [&](
		int from, int to, int dim
	) -> std::string {
		if (dim == int(shape.size())) {
			std::string buf;
			for (int i = from; i < to; ++i) {
				auto ind = posToIdx(i, shape);
				auto p = value.find(ind);
				if (p == value.end()) {
					buf += zero.getType().toLLVMIR() + " " + zero.toLLVMIR() + ", ";
				} else {
					buf += p->second.getType().toLLVMIR() +
						" " + p->second.toLLVMIR() + ", ";
				}
			}
			buf.pop_back();
			buf.pop_back();
			return buf;
		}
		std::vector<int> nowShape;
		for (int i = dim; i < int(shape.size()); ++i) {
			nowShape.push_back(shape[i]);
		}
		auto buf = std::string();
		if (dim) {
			IntArrayType fat(nowShape);
			buf += fat.toLLVMIR();
		}
		buf += "[";
		int sz = 1;
		for (int i = dim + 1; i < int(shape.size()); ++i) { sz *= shape[i]; }
		for (int i = 0; i < shape[dim]; ++i) {
			buf += fun(from + sz * i, from + sz * (i + 1), dim + 1) + ", ";
		}
		buf.pop_back();
		buf.pop_back();
		buf += "]";
		return buf;
	};
	int sz = 1;
	for (int x: shape) { sz *= x; }
	return fun(0, sz, 0);
}

std::unique_ptr<StaticValue>
IntArrayStaticValue::getValue(const VI & ind) const {
	if (ind.size() == shape.size()) {
		auto p = value.find(ind);
		if (p == value.end()) {
			return zero.getValue({ });
		} else {
			return p->second.getValue({ });
		}
	} else {
		com::TODO("Maybe do not need this.", CODEPOS);
	}
}

IntArrayStaticValue::IntArrayStaticValue(VI _shape) :
	StaticValue(sup::IntArrayType(_shape)), shape(std::move(_shape)) {
}

std::unique_ptr<moeconcept::Cutable> IntArrayStaticValue::_cutToUniquePtr() {
	return std::make_unique<IntArrayStaticValue>(std::move(*this));
}

void IntArrayStaticValue::insertValue(const VI & idx, StaticValue & staticValue) {
	com::Assert(idx.size() == shape.size(), "", CODEPOS);
	auto pSV = com::dynamic_cast_uPtr<IntStaticValue>(convertOnSV(staticValue, IntType()));
	value.emplace(idx, std::move(*pSV));
}

std::unique_ptr<moeconcept::Cloneable>
BoolStaticValue::_cloneToUniquePtr() const {
	return std::make_unique<BoolStaticValue>(*this);
}

BoolStaticValue::BoolStaticValue(const std::string & literal)
	: StaticValue(BoolType()), value(std::stoi(literal)) {
}

BoolStaticValue::BoolStaticValue(bool value)
	: StaticValue(BoolType()), value(value) {
}

std::string BoolStaticValue::toLLVMIR() const {
	return intToString(value);
}

PointerStaticValue::PointerStaticValue(const PointerType & pointerType) :
	StaticValue(pointerType) {
}

std::string PointerStaticValue::toLLVMIR() const {
	return "null";
}

std::unique_ptr<StaticValue> PointerStaticValue::getValue(const VI &) const {
	com::Throw("", CODEPOS);
}

[[nodiscard]] std::unique_ptr<StaticValue>
BoolStaticValue::getValue(const VI &) const {
	return com::dynamic_cast_uPtr<StaticValue>(cloneToUniquePtr());
}

void BoolStaticValue::insertValue(const sup::VI & idx, sup::StaticValue & staticValue) {
	auto pSV = com::dynamic_cast_uPtr<BoolStaticValue>(convertOnSV(staticValue, BoolType()));
	value = pSV->value;
}

std::unique_ptr<StaticValue> BoolStaticValue::calc(
	const std::string & op, const StaticValue & oVal
) const {
	std::unique_ptr<StaticValue> upSVRes;
	switch (oVal.getType().type) {
		case Type::Float_t: {
			upSVRes = calcOfFloat(
				float(value),
				dynamic_cast<const FloatStaticValue &>(oVal).value, op
			);
			break;
		}
		case Type::Int_t: {
			upSVRes = calcOfInt(
				value,
				dynamic_cast<const IntStaticValue &>(oVal).value, op
			);
			break;
		}
		case Type::Bool_t: {
			upSVRes = calcOfBool(
				value,
				dynamic_cast<const BoolStaticValue &>(oVal).value, op
			);
			break;
		}
		default: {
			com::Throw("Type of oVal should be Int_t/Float_t/Bool_t.", CODEPOS);
		}
	}
	return upSVRes;
}

std::unique_ptr<StaticValue>
BoolStaticValue::calc(const std::string & op) const {
	return calcOfBool(false, value, op);
}

std::unique_ptr<moeconcept::Cutable> BoolStaticValue::_cutToUniquePtr() {
	return std::make_unique<BoolStaticValue>(std::move(*this));
}

std::unique_ptr<StaticValue>
convertOnSV(const StaticValue & from, const TypeInfo & to) {
	auto typeFrom = from.getType().type, typeTo = to.type;
	auto res = std::unique_ptr<StaticValue>(nullptr);
	switch (typeFrom) {
		case Type::Int_t: {
			auto value = dynamic_cast<const IntStaticValue &>(from).value;
			switch (typeTo) {
				case Type::Int_t: {
					res = std::make_unique<IntStaticValue>(int(value));
					break;
				}
				case Type::Float_t: {
					res = std::make_unique<FloatStaticValue>(float(value));
					break;
				}
				case Type::Bool_t: {
					res = std::make_unique<BoolStaticValue>(bool(value));
				}
				default: { break; }
			}
			break;
		}
		case Type::Float_t: {
			auto value = dynamic_cast<const FloatStaticValue &>(from).value;
			switch (typeTo) {
				case Type::Int_t: {
					res = std::make_unique<IntStaticValue>(int(value));
					break;
				}
				case Type::Float_t: {
					res = std::make_unique<FloatStaticValue>(float(value));
					break;
				}
				case Type::Bool_t: {
					res = std::make_unique<BoolStaticValue>(bool(value));
				}
				default: { break; }
			}
			break;
		}
		case Type::Bool_t: {
			auto value = dynamic_cast<const BoolStaticValue &>(from).value;
			switch (typeTo) {
				case Type::Int_t: {
					res = std::make_unique<IntStaticValue>(int(value));
					break;
				}
				case Type::Float_t: {
					res = std::make_unique<FloatStaticValue>(float(value));
					break;
				}
				case Type::Bool_t: {
					res = std::make_unique<BoolStaticValue>(bool(value));
				}
				default: { break; }
			}
			break;
		}
		default: break;
	}
	if (!res) {
		com::Throw("Unsupported static type conversion.", CODEPOS);
	} else {
		return res;
	}
}

std::unique_ptr<StaticValue> CalcOnStaticValue::operator()(
	const StaticValue & opL, const std::string & op, const StaticValue & opR
) const {
	return opL.calc(op, opR);
}

std::unique_ptr<StaticValue>
CalcOnStaticValue::operator()(
	const std::string & op, const StaticValue & opR
) const {
	return opR.calc(op);
}

CalcOnStaticValue calcOnSV;

std::unique_ptr<StaticValue>
fromTypeInfoToStaticValue(const TypeInfo & typeInfo) {
	switch (typeInfo.type) {
		case Type::Int_t: {
			return std::make_unique<IntStaticValue>();
		}
		case Type::Float_t: {
			return std::make_unique<FloatStaticValue>();
		}
		case Type::IntArray_t: {
			auto viTypeInfo = dynamic_cast<const IntArrayType &>(typeInfo);
			return std::make_unique<IntArrayStaticValue>(viTypeInfo.shape);
		}
		case Type::FloatArray_t: {
			auto vfTypeInfo = dynamic_cast<const FloatArrayType &>(typeInfo);
			return std::make_unique<FloatArrayStaticValue>(vfTypeInfo.shape);
		}
		case Type::Bool_t: {
			return std::make_unique<BoolStaticValue>();
		}
		case Type::Pointer_t: {
			const auto & pointerType = dynamic_cast<const PointerType &>(typeInfo);
			return std::make_unique<PointerStaticValue>(pointerType);
		}
		default: com::Throw("", CODEPOS);
	}
}

std::unique_ptr<StaticValue>
calcOfSV(const StaticValue & left, const StaticValue & right, const std::string & op) {
	com::Assert(
		left.getType() == right.getType() &&
			com::enum_fun::in(
				left.getType().type, {sup::Type::Float_t, sup::Type::Int_t}
			),
		"", CODEPOS
	);
	if (left.getType().type == sup::Type::Int_t) {
		auto iLeft = dynamic_cast<const IntStaticValue &>(left);
		auto iRight = dynamic_cast<const IntStaticValue &>(right);
		return calcOfInt(iLeft.value, iRight.value, op);
	} else if (left.getType().type == sup::Type::Float_t) {
		auto fLeft = dynamic_cast<const FloatStaticValue &>(left);
		auto fRight = dynamic_cast<const FloatStaticValue &>(right);
		return calcOfFloat(fLeft.value, fRight.value, op);
	} else { return nullptr; }
}

}
