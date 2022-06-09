#include "StaticValue.hpp"
#include "stlextension.hpp"

ircode::StaticValue::StaticValue(const ircode::TypeInfo & typeInfo)
		: uPtrInfo(
		com::dynamic_cast_unique_ptr<moeconcept::Cloneable, TypeInfo>(
				typeInfo.cloneToUniquePtr()
		)) {
}

ircode::StaticValue::StaticValue(const ircode::StaticValue & staticValue)
		: uPtrInfo(
		com::dynamic_cast_unique_ptr<moeconcept::Cloneable, TypeInfo>(
				staticValue.uPtrInfo->cloneToUniquePtr())
) {
}

ircode::StaticValue::StaticValue() : uPtrInfo(nullptr) {
}

std::unique_ptr<moeconcept::Cloneable>
ircode::FloatStaticValue::_cloneToUniquePtr() const {
	return std::make_unique<FloatStaticValue>(*this);
}

ircode::FloatStaticValue::FloatStaticValue(const std::string & literal)
		: StaticValue(FloatType()), value(std::stof(literal)) {
}

std::string ircode::FloatStaticValue::toLLVMIR() const {
	return uPtrInfo->toLLVMIR() + " " + floatToString(value);
}

std::unique_ptr<ircode::StaticValue>
ircode::FloatStaticValue::getValue(const std::vector<int> &) const {
	return com::dynamic_cast_unique_ptr<moeconcept::Cloneable, StaticValue>(
			cloneToUniquePtr());
}

std::unique_ptr<moeconcept::Cloneable>
ircode::IntStaticValue::_cloneToUniquePtr() const {
	return std::make_unique<IntStaticValue>(*this);
}

ircode::IntStaticValue::IntStaticValue(const std::string & literal)
		: StaticValue(IntType()), value(std::stoi(literal)) {
}

std::string ircode::IntStaticValue::toLLVMIR() const {
	return uPtrInfo->toLLVMIR() + " " + intToString(value);
}

std::unique_ptr<ircode::StaticValue>
ircode::IntStaticValue::getValue(const std::vector<int> &) const {
	return com::dynamic_cast_unique_ptr<moeconcept::Cloneable, StaticValue>(
			cloneToUniquePtr());
}

std::unique_ptr<moeconcept::Cloneable>
ircode::FloatArrayStaticValue::_cloneToUniquePtr() const {
	return std::make_unique<FloatArrayStaticValue>(*this);
}

ircode::FloatArrayStaticValue::FloatArrayStaticValue(
		int len, std::vector<FloatStaticValue> vi
)
		: StaticValue(), shape({len}), value(std::move(vi)) {
	StaticValue::uPtrInfo = std::make_unique<FloatArrayType>(shape);
}

ircode::FloatArrayStaticValue::FloatArrayStaticValue(
		int len, const std::vector<int> & preShape,
		const std::vector<FloatArrayStaticValue> & vi
) {
	com::Assert(len <= (int) vi.size(), "len should less than vi.size().",
	            CODEPOS);
	shape = {len};
	stlextension::vector::PushBackByIterators(shape, preShape.begin(),
	                                          preShape.end());
	for (const auto & fsa : vi) {
		if (fsa.shape != preShape) {
			com::Throw("Shape not same.", CODEPOS);
		}
		stlextension::vector::PushBackByIterators(value, fsa.value.begin(),
		                                          fsa.value.end());
	}
	int sz = 1;
	for (int x : preShape) sz *= x;
	stlextension::vector::PushBackByNumberAndInstance(
			value, (len - int(vi.size()) * sz), FloatStaticValue());
	StaticValue::uPtrInfo = std::make_unique<FloatArrayType>(shape);
}

std::string ircode::FloatArrayStaticValue::toLLVMIR() const {
	std::function<std::string(int, int, int)> fun = [&](
			int from, int to, int dim
	) -> std::string {
		if (dim == int(shape.size())) {
			std::string buf;
			for (int i = from; i < to; ++i) {
				buf += value[i].toLLVMIR() + ", ";
			}
			buf.pop_back();
			buf.pop_back();
			return buf;
		}
		std::vector<int> nowShape;
		for (int i = dim; i < int(shape.size()); ++i)
			nowShape.push_back(shape[i]);
		FloatArrayType fat(nowShape);
		std::string buf = fat.toLLVMIR() + " [";
		int sz = 1;
		for (int i = dim + 1; i < int(shape.size()); ++i) sz *= shape[i];
		for (int i = 0; i < shape[dim]; ++i) {
			buf += fun(from + sz * i, from + sz * (i + 1), dim + 1) + ", ";
		}
		buf.pop_back();
		buf.pop_back();
		return buf + "]";
	};
	int sz = 1;
	for (int x : shape) sz *= x;
	return fun(0, sz, 0);
}

std::unique_ptr<ircode::StaticValue>
ircode::FloatArrayStaticValue::getValue(const std::vector<int> & ind) const {
	if (ind.size() == shape.size()) {
		size_t idx = 0, stride = 1;
		for (auto itInd = ind.rbegin(), itShape = shape.rbegin();
		     itInd != ind.rend(); ++itInd, ++itShape) {
			idx += *itInd * stride;
			stride *= *itShape;
		}
		return com::dynamic_cast_unique_ptr<moeconcept::Cloneable, StaticValue>(
				value[idx].cloneToUniquePtr());
	} else {
		com::TODO("Maybe do not need this.", CODEPOS);
	}
}

std::unique_ptr<moeconcept::Cloneable>
ircode::IntArrayStaticValue::_cloneToUniquePtr() const {
	return std::make_unique<IntArrayStaticValue>(*this);
}

ircode::IntArrayStaticValue::IntArrayStaticValue(
		int len, std::vector<IntStaticValue> vi
) : StaticValue(), shape({len}), value(std::move(vi)) {
	StaticValue::uPtrInfo = std::make_unique<IntArrayType>(shape);
}

ircode::IntArrayStaticValue::IntArrayStaticValue(
		int len, const std::vector<int> & preShape,
		const std::vector<IntArrayStaticValue> & vi
) {
	
	com::Assert(len <= (int) vi.size(), "len should less than vi.size().",
	            CODEPOS);
	shape = {len};
	stlextension::vector::PushBackByIterators(shape, preShape.begin(),
	                                          preShape.end());
	for (const auto & fsa : vi) {
		if (fsa.shape != preShape) {
			com::Throw("Shape not same.", CODEPOS);
		}
		stlextension::vector::PushBackByIterators(value, fsa.value.begin(),
		                                          fsa.value.end());
	}
	int sz = 1;
	for (int x : preShape) sz *= x;
	stlextension::vector::PushBackByNumberAndInstance(
			value, (len - int(vi.size()) * sz), IntStaticValue());
	StaticValue::uPtrInfo = std::make_unique<IntArrayType>(shape);
}

std::string ircode::IntArrayStaticValue::toLLVMIR() const {
	std::function<std::string(int, int, int)> fun = [&](
			int from, int to, int dim
	) -> std::string {
		if (dim == int(shape.size())) {
			std::string buf;
			for (int i = from; i < to; ++i) {
				buf += value[i].toLLVMIR() + ", ";
			}
			buf.pop_back();
			buf.pop_back();
			return buf;
		}
		std::vector<int> nowShape;
		for (int i = dim; i < int(shape.size()); ++i)
			nowShape.push_back(shape[i]);
		FloatArrayType fat(nowShape);
		std::string buf = fat.toLLVMIR() + " [";
		int sz = 1;
		for (int i = dim + 1; i < int(shape.size()); ++i) sz *= shape[i];
		for (int i = 0; i < shape[dim]; ++i) {
			buf += fun(from + sz * i, from + sz * (i + 1), dim + 1) + ", ";
		}
		buf.pop_back();
		buf.pop_back();
		return buf + "]";
	};
	int sz = 1;
	for (int x : shape) sz *= x;
	return fun(0, sz, 0);
}

std::unique_ptr<ircode::StaticValue>
ircode::IntArrayStaticValue::getValue(const std::vector<int> & ind) const {
	if (ind.size() == shape.size()) {
		size_t idx = 0, stride = 1;
		for (auto itInd = ind.rbegin(), itShape = shape.rbegin();
		     itInd != ind.rend(); ++itInd, ++itShape) {
			idx += *itInd * stride;
			stride *= *itShape;
		}
		return com::dynamic_cast_unique_ptr<moeconcept::Cloneable, StaticValue>(
				value[idx].cloneToUniquePtr());
	} else {
		com::TODO("Maybe do not need this.", CODEPOS);
	}
}
