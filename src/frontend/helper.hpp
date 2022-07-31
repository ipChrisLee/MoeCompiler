#pragma once

#include <string>

#include <common.hpp>
#include <utility>

#include "mir/Instr.hpp"
#include "mir/Module.hpp"


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

	bool isAll0AfterNDim(int n);

	int getPos() const;

	int getStride() const;

	std::string idxToStr() const;
};

template<typename T>
struct ArrayItem {
	//  `T` is `std::unique_ptr<sup::StaticValue>` for const array and global array.
	//  `T` is `mir::AddrOperand *` for local non-const array.
	std::vector<int> idx;
	T val;
	/**
	 * @brief Instructions to init value in position idx.
	 * @note For staticValue init, this is empty.
	 * @whyNeedIt int ar[3]={0,ar[0],ar[1]};
	 */
	std::list<mir::Instr *> instrsToInit;

	ArrayItem(
		std::vector<int> idx, T && val,
		std::list<mir::Instr *> instrsToInit = { }
	) : idx(std::move(idx)), val(std::move(val)),
	    instrsToInit(std::move(instrsToInit)) {}

	int getPos(const std::vector<int> & shape) const {
		int step = 1, pos = 0;
		for (auto iIdx = idx.rbegin(), iShape = shape.rbegin(); iIdx != idx.rend();
		     ++iIdx, ++iShape) {
			pos += *iIdx * step;
			step *= *iShape;
		}
		return pos;
	}
};


std::list<mir::Instr *> fromArrayItemsToInstrs(
	mir::Module & ir,
	std::vector<ArrayItem<mir::AddrOperand *>> && items,
	const std::vector<int> & shape,
	mir::AddrVariable * varMemBaseAddr,
	const sup::TypeInfo & typeOfElement
);

std::unique_ptr<sup::StaticValue> fromArrayItemsToStaticValue(
	mir::Module & ir,
	const std::vector<ArrayItem<std::unique_ptr<sup::StaticValue>>> & items,
	const std::vector<int> & shape,
	const sup::TypeInfo & typeOfElement
);

}
