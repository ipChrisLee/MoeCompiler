#pragma once

#include <string>

#include <common.hpp>
#include <utility>

#include "IR/IRInstr.hpp"
#include "IR/IRModule.hpp"
#include "support/Idx.hpp"


namespace frontend {

int shapeToStride(const std::vector<int> & shape);

struct IdxView {
	std::vector<int> idx;
	std::vector<int> shape;

	explicit IdxView(std::vector<int> _shape = { });

	IdxView(IdxView &&) = default;

	IdxView & operator=(IdxView && idxView) = default;

	void addOnDimN(int n, int a = 1);
	void set0AfterNDim(int n);
	void set0AfterNDimAndCarry(int n);
	bool isAll0AfterNDim(int n);
	int getPos() const;
	int getStride() const;
};

template<typename T>
struct ArrayItem {
	//  `T` is `std::unique_ptr<sup::StaticValue>` for const array and global array.
	//  `T` is `ircode::AddrOperand *` for local non-const array.
	std::vector<int> idx;

	bool operator<(const ArrayItem & other) const {
		return idx < other.idx;
	}

	T val;
	/**
	 * @brief Instructions to init value in position idx.
	 * @note For staticValue init, this is empty.
	 * @whyNeedIt int ar[3]={0,ar[0],ar[1]};
	 */
	std::list<ircode::IRInstr *> instrsToInit;

	ArrayItem(
		std::vector<int> idx, T && val,
		std::list<ircode::IRInstr *> instrsToInit = { }
	) : idx(std::move(idx)), val(std::move(val)),
	    instrsToInit(std::move(instrsToInit)) {
	}

	int getPos(const sup::VI & shape) const {
		return sup::idxToPos(idx, shape);
	}
};


std::list<ircode::IRInstr *> fromArrayItemsToInstrs(
	ircode::IRModule & ir,
	std::set<ArrayItem<ircode::AddrOperand *>> && items,
	const std::vector<int> & shape,
	ircode::AddrVariable * varMemBaseAddr,
	const sup::TypeInfo & typeOfElement
);

std::unique_ptr<sup::StaticValue> fromArrayItemsToStaticValue(
	ircode::IRModule & ir,
	const std::set<ArrayItem<std::unique_ptr<sup::StaticValue>>> & items,
	const std::vector<int> & shape,
	const sup::TypeInfo & typeOfElement
);

}
