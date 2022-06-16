#include <cmath>

#include "StaticValue.hpp"
#include "stlextension.hpp"

#include "frontend/TypeInfo.hpp"
#include "frontendHeader.hpp"

std::unique_ptr<ircode::StaticValue>
ircode::calcOfFloat(float fl, float fr, const std::string & op) {
	std::unique_ptr<ircode::StaticValue> upSV;
	com::regSwitch(
		op, {
			{
				"\\*",    [&upSV, fl, fr]() {
				upSV = std::make_unique<ircode::FloatStaticValue>(fl * fr);
			}},
			{
				"\\-",    [&upSV, fl, fr]() {
				upSV = std::make_unique<ircode::FloatStaticValue>(fl - fr);
			}},
			{
				"\\+",    [&upSV, fl, fr]() {
				upSV = std::make_unique<ircode::FloatStaticValue>(fl + fr);
			}},
			{
				"\\/",    [&upSV, fl, fr]() {
				upSV = std::make_unique<ircode::FloatStaticValue>(fl / fr);
			}},
			{
				"%",      []() {
				com::Throw("`float % float` is illegal!", CODEPOS);
			}},
			{
				"&&",     [&upSV, fl, fr]() {
				upSV = std::make_unique<ircode::BoolStaticValue>(fl && fr);
			}},
			{
				"\\|\\|", [&upSV, fl, fr]() {
				upSV = std::make_unique<ircode::BoolStaticValue>(fl || fr);
			}},
			{
				"<",      [&upSV, fl, fr]() {
				upSV = std::make_unique<ircode::BoolStaticValue>(fl < fr);
			}},
			{
				">",      [&upSV, fl, fr]() {
				upSV = std::make_unique<ircode::BoolStaticValue>(fl > fr);
			}},
			{
				"<=",     [&upSV, fl, fr]() {
				upSV = std::make_unique<ircode::BoolStaticValue>(fl <= fr);
			}},
			{
				">=",     [&upSV, fl, fr]() {
				upSV = std::make_unique<ircode::BoolStaticValue>(fl >= fr);
			}},
			{
				"==",     [&upSV, fl, fr]() {
				upSV = std::make_unique<ircode::BoolStaticValue>(fl == fr);
			}},
			{
				"!=",     [&upSV, fl, fr]() {
				upSV = std::make_unique<ircode::BoolStaticValue>(fl != fr);
			}},
			{
				"!",      [&upSV, fr]() {
				upSV = std::make_unique<ircode::BoolStaticValue>(!fr);
			}},
		}
	);
	return upSV;
}

std::unique_ptr<ircode::StaticValue>
ircode::calcOfInt(int il, int ir, const std::string & op) {
	std::unique_ptr<ircode::StaticValue> upSV;
	com::regSwitch(
		op, {
			{
				"\\*",    [&upSV, il, ir]() {
				upSV = std::make_unique<ircode::IntStaticValue>(il * ir);
			}},
			{
				"\\-",    [&upSV, il, ir]() {
				upSV = std::make_unique<ircode::IntStaticValue>(il - ir);
			}},
			{
				"\\+",    [&upSV, il, ir]() {
				upSV = std::make_unique<ircode::IntStaticValue>(il + ir);
			}},
			{
				"\\/",    [&upSV, il, ir]() {
				upSV = std::make_unique<ircode::IntStaticValue>(il / ir);
			}},
			{
				"%",      [&upSV, il, ir]() {
				upSV = std::make_unique<ircode::IntStaticValue>(il % ir);
			}},
			{
				"&&",     [&upSV, il, ir]() {
				upSV = std::make_unique<ircode::BoolStaticValue>(il && ir);
			}},
			{
				"\\|\\|", [&upSV, il, ir]() {
				upSV = std::make_unique<ircode::BoolStaticValue>(il || ir);
			}},
			{
				"<",      [&upSV, il, ir]() {
				upSV = std::make_unique<ircode::BoolStaticValue>(il < ir);
			}},
			{
				">",      [&upSV, il, ir]() {
				upSV = std::make_unique<ircode::BoolStaticValue>(il > ir);
			}},
			{
				"<=",     [&upSV, il, ir]() {
				upSV = std::make_unique<ircode::BoolStaticValue>(il <= ir);
			}},
			{
				">=",     [&upSV, il, ir]() {
				upSV = std::make_unique<ircode::BoolStaticValue>(il >= ir);
			}},
			{
				"==",     [&upSV, il, ir]() {
				upSV = std::make_unique<ircode::BoolStaticValue>(il == ir);
			}},
			{
				"!=",     [&upSV, il, ir]() {
				upSV = std::make_unique<ircode::BoolStaticValue>(il != ir);
			}},
			{
				"!",      [&upSV, ir]() {
				upSV = std::make_unique<ircode::BoolStaticValue>(!ir);
			}},
		}
	);
	return upSV;
}

std::unique_ptr<ircode::StaticValue>
ircode::calcOfBool(bool bl, bool br, const std::string & op) {
	std::unique_ptr<ircode::StaticValue> upSV;
	com::regSwitch(
		op, {
			{
				"\\*",    [&upSV, bl, br]() {
				upSV = std::make_unique<ircode::IntStaticValue>(bl * br);
			}},
			{
				"\\-",    [&upSV, bl, br]() {
				upSV = std::make_unique<ircode::IntStaticValue>(bl - br);
			}},
			{
				"\\+",    [&upSV, bl, br]() {
				upSV = std::make_unique<ircode::IntStaticValue>(bl + br);
			}},
			{
				"\\/",    [&upSV, bl, br]() {
				upSV = std::make_unique<ircode::IntStaticValue>(bl / br);
			}},
			{
				"%",      [&upSV, bl, br]() {
				upSV = std::make_unique<ircode::IntStaticValue>(bl % br);
			}},
			{
				"&&",     [&upSV, bl, br]() {
				upSV = std::make_unique<ircode::BoolStaticValue>(bl && br);
			}},
			{
				"\\|\\|", [&upSV, bl, br]() {
				upSV = std::make_unique<ircode::BoolStaticValue>(bl || br);
			}},
			{
				"<",      [&upSV, bl, br]() {
				upSV = std::make_unique<ircode::BoolStaticValue>(bl < br);
			}},
			{
				">",      [&upSV, bl, br]() {
				upSV = std::make_unique<ircode::BoolStaticValue>(bl > br);
			}},
			{
				"<=",     [&upSV, bl, br]() {
				upSV = std::make_unique<ircode::BoolStaticValue>(bl <= br);
			}},
			{
				">=",     [&upSV, bl, br]() {
				upSV = std::make_unique<ircode::BoolStaticValue>(bl >= br);
			}},
			{
				"==",     [&upSV, bl, br]() {
				upSV = std::make_unique<ircode::BoolStaticValue>(bl == br);
			}},
			{
				"!=",     [&upSV, bl, br]() {
				upSV = std::make_unique<ircode::BoolStaticValue>(bl != br);
			}},
			{
				"!",      [&upSV, br]() {
				upSV = std::make_unique<ircode::BoolStaticValue>(!br);
			}},
		}
	);
	return upSV;
}

std::unique_ptr<ircode::StaticValue>
ircode::zeroExtensionValueOfType(const ircode::TypeInfo & typeInfo) {
	switch (typeInfo.type) {
		case ircode::TypeInfo::Type::Int_t: {
			return std::make_unique<ircode::IntStaticValue>();
		}
		case ircode::TypeInfo::Type::Float_t: {
			return std::make_unique<ircode::FloatStaticValue>();
		}
		case ircode::TypeInfo::Type::IntArray_t: {
			return std::make_unique<ircode::IntArrayStaticValue>(
				dynamic_cast<const ircode::IntArrayType &>(typeInfo).shape
			);
		}
		case ircode::TypeInfo::Type::FloatArray_t: {
			return std::make_unique<ircode::FloatArrayStaticValue>(
				dynamic_cast<const ircode::FloatArrayType &>(typeInfo).shape
			);
		}
		default: {
			com::Throw("", CODEPOS);
		}
	}
}

ircode::StaticValue::StaticValue(const ircode::TypeInfo & typeInfo)
	: uPtrInfo(com::dynamic_cast_uPtr<TypeInfo>(typeInfo.cloneToUniquePtr())) {
}

ircode::StaticValue::StaticValue(const ircode::StaticValue & staticValue)
	: uPtrInfo(
	com::dynamic_cast_uPtr<TypeInfo>(staticValue.uPtrInfo->cloneToUniquePtr())
) {
}

std::unique_ptr<ircode::StaticValue>
ircode::StaticValue::calc(
	const ircode::StaticValue &, const std::string & op
) const {
	com::Throw("This method should not be invoked.", CODEPOS);
}

std::unique_ptr<ircode::StaticValue>
ircode::StaticValue::calc(const std::string & op) const {
	com::Throw("This method should not be invoked.", CODEPOS);
}

std::unique_ptr<ircode::StaticValue>
ircode::StaticValue::convertTo(const ircode::TypeInfo &) {
	com::Throw("Cannot convert to other type from this type.", CODEPOS);
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
	return com::dynamic_cast_uPtr<StaticValue>(
		cloneToUniquePtr());
}

std::unique_ptr<ircode::StaticValue> ircode::FloatStaticValue::calc(
	const ircode::StaticValue & oVal, const std::string & op
) const {
	std::unique_ptr<ircode::StaticValue> upSVRes;
	switch (oVal.getTypeInfo().type) {
		case TypeInfo::Type::Float_t: {
			upSVRes = calcOfFloat(
				value,
				dynamic_cast<const ircode::FloatStaticValue &>(oVal).value, op
			);
			break;
		}
		case TypeInfo::Type::Int_t: {
			upSVRes = calcOfFloat(
				value,
				float(dynamic_cast<const ircode::IntStaticValue &>(oVal).value), op
			);
			break;
		}
		case TypeInfo::Type::Bool_t: {
			upSVRes = calcOfFloat(
				value,
				float(dynamic_cast<const ircode::BoolStaticValue &>(oVal).value),
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

ircode::FloatStaticValue::FloatStaticValue(float value)
	: StaticValue(FloatType()), value(value) {
}

std::unique_ptr<ircode::StaticValue>
ircode::FloatStaticValue::calc(const std::string & op) const {
	return calcOfFloat(0, value, op);
}

std::unique_ptr<ircode::StaticValue>
ircode::FloatStaticValue::convertTo(const ircode::TypeInfo & toType) {
	switch (toType.type) {
		case TypeInfo::Type::Bool_t: {
			return std::make_unique<ircode::BoolStaticValue>(bool(value));
		}
		case TypeInfo::Type::Int_t: {
			return std::make_unique<ircode::IntStaticValue>(int(value));
		}
		case TypeInfo::Type::Float_t: {
			return std::make_unique<ircode::FloatStaticValue>(float(value));
		}
		default: {
			com::Throw("Conversion failed since toType unsupported.", CODEPOS);
		}
	}
}

std::unique_ptr<moeconcept::Cloneable>
ircode::IntStaticValue::_cloneToUniquePtr() const {
	return std::make_unique<IntStaticValue>(*this);
}

ircode::IntStaticValue::IntStaticValue(const std::string & literal)
	: StaticValue(IntType()), value(ircode::literalToInt(literal)) {
}

std::string ircode::IntStaticValue::toLLVMIR() const {
	return uPtrInfo->toLLVMIR() + " " + intToString(value);
}

std::unique_ptr<ircode::StaticValue>
ircode::IntStaticValue::getValue(const std::vector<int> &) const {
	return com::dynamic_cast_uPtr<StaticValue>(
		cloneToUniquePtr());
}

ircode::IntStaticValue::IntStaticValue(int value)
	: StaticValue(IntType()), value(value) {
}

std::unique_ptr<ircode::StaticValue> ircode::IntStaticValue::calc(
	const ircode::StaticValue & oVal, const std::string & op
) const {
	std::unique_ptr<ircode::StaticValue> upSVRes;
	switch (oVal.getTypeInfo().type) {
		case TypeInfo::Type::Float_t: {
			upSVRes = calcOfFloat(
				float(value),
				dynamic_cast<const ircode::FloatStaticValue &>(oVal).value, op
			);
			break;
		}
		case TypeInfo::Type::Int_t: {
			upSVRes = calcOfInt(
				value,
				dynamic_cast<const ircode::IntStaticValue &>(oVal).value, op
			);
			break;
		}
		case TypeInfo::Type::Bool_t: {
			upSVRes = calcOfInt(
				value,
				dynamic_cast<const ircode::BoolStaticValue &>(oVal).value, op
			);
			break;
		}
		default: {
			com::Throw("Type of oVal should be Int_t/Float_t/Bool_t.", CODEPOS);
		}
	}
	return upSVRes;
}

std::unique_ptr<ircode::StaticValue>
ircode::IntStaticValue::calc(const std::string & op) const {
	return calcOfInt(0, value, op);
}

std::unique_ptr<ircode::StaticValue>
ircode::IntStaticValue::convertTo(const ircode::TypeInfo & toType) {
	switch (toType.type) {
		case TypeInfo::Type::Bool_t: {
			return std::make_unique<ircode::BoolStaticValue>(bool(value));
		}
		case TypeInfo::Type::Int_t: {
			return std::make_unique<ircode::IntStaticValue>(int(value));
		}
		case TypeInfo::Type::Float_t: {
			return std::make_unique<ircode::FloatStaticValue>(float(value));
		}
		default: {
			com::Throw("Conversion failed since toType unsupported.", CODEPOS);
		}
	}
}

std::unique_ptr<moeconcept::Cloneable>
ircode::FloatArrayStaticValue::_cloneToUniquePtr() const {
	return std::make_unique<FloatArrayStaticValue>(*this);
}

ircode::FloatArrayStaticValue::FloatArrayStaticValue(
	int len, std::vector<FloatStaticValue> vi
) : StaticValue(ircode::FloatArrayType({len})), shape({len}), value(std::move(vi)) {
	stlextension::vector::PushBackByNumberAndInstance(
		value, len - int(value.size()), ircode::FloatStaticValue()
	);
}

ircode::FloatArrayStaticValue::FloatArrayStaticValue(
	int len, const std::vector<int> & preShape,
	const std::vector<FloatArrayStaticValue> & vi
) {
	com::Assert(
		len >= (int) vi.size(), "len should less than vi.size().",
		CODEPOS
	);
	shape = {len};
	stlextension::vector::PushBackByIterators(
		shape, preShape.begin(),
		preShape.end());
	for (const auto & fsa : vi
		) {
		if (fsa.shape != preShape) {
			com::Throw("Shape not same.", CODEPOS);
		}
		stlextension::vector::PushBackByIterators(
			value, fsa.value.begin(),
			fsa.value.end());
	}
	int sz = 1;
	for (int x : preShape) { sz *= x; }
	stlextension::vector::PushBackByNumberAndInstance(
		value, (len - int(vi.size())) * sz, FloatStaticValue());
	StaticValue::uPtrInfo = std::make_unique<FloatArrayType>(shape);
}

std::string ircode::FloatArrayStaticValue::toLLVMIR() const {
	std::function<std::string(int, int, int)> fun = [&](
		int from, int to, int dim
	) -> std::string {
		if (dim == int(shape.size())) {
			std::string buf;
			for (int i = from; i < to; ++i
				) {
				buf += value[i].toLLVMIR() + ", ";
			}
			buf.pop_back();
			buf.pop_back();
			return buf;
		}
		std::vector<int> nowShape;
		for (int i = dim; i < int(shape.size()); ++i
			) {
			nowShape.push_back(shape[i]);
		}
		FloatArrayType fat(nowShape);
		std::string buf = fat.toLLVMIR() + " [";
		int sz = 1;
		for (int i = dim + 1; i < int(shape.size()); ++i) { sz *= shape[i]; }
		for (int i = 0; i < shape[dim]; ++i
			) {
			buf += fun(from + sz * i, from + sz * (i + 1), dim + 1) + ", ";
		}
		buf.pop_back();
		buf.pop_back();
		return buf + "]";
	};
	int sz = 1;
	for (int x : shape) { sz *= x; }
	return fun(0, sz, 0);
}

std::unique_ptr<ircode::StaticValue>
ircode::FloatArrayStaticValue::getValue(const std::vector<int> & ind) const {
	if (ind.size() == shape.size()) {
		size_t idx = 0, stride = 1;
		for (auto itInd = ind.rbegin(), itShape = shape.rbegin();
		     itInd != ind.rend(); ++itInd, ++itShape
			) {
			idx += *itInd * stride;
			stride *= *itShape;
		}
		return com::dynamic_cast_uPtr<StaticValue>(
			value[idx].cloneToUniquePtr());
	} else {
		com::TODO("Maybe do not need this.", CODEPOS);
	}
}

ircode::FloatArrayStaticValue::FloatArrayStaticValue(
	int len, const std::vector<int> & preShape,
	const std::vector<FloatArrayStaticValue> & vi,
	const std::vector<FloatStaticValue> & rest
) {
	int sz = 1;
	for (int x : preShape) { sz *= x; }
	com::Assert(
		len >= (int) vi.size() + ((int) rest.size() + sz - 1) / sz,
		"len should greater than vi.size()+lower_bound(rest.size()/sz).",
		CODEPOS
	);
	shape = {len};
	stlextension::vector::PushBackByIterators(
		shape, preShape.begin(),
		preShape.end()
	);
	for (const auto & fsa : vi) {
		if (fsa.shape != preShape) {
			com::Throw("Shape not same.", CODEPOS);
		}
		stlextension::vector::PushBackByIterators(
			value, fsa.value.begin(),
			fsa.value.end()
		);
	}
	stlextension::vector::PushBackByIterators(
		value, rest.begin(), rest.end()
	);
	stlextension::vector::PushBackByNumberAndInstance(
		value,
		len * sz - int(vi.size()) * sz - int(rest.size()),
		FloatStaticValue()
	);
	StaticValue::uPtrInfo = std::make_unique<FloatArrayType>(shape);
}

ircode::FloatArrayStaticValue::FloatArrayStaticValue(std::vector<int> shape) : shape(
	std::move(shape)) {
	int sz = 1;
	for (auto l : this->shape) {
		sz *= l;
	}
	stlextension::vector::PushBackByNumberAndInstance(
		value, sz, ircode::FloatStaticValue()
	);
	
}

std::unique_ptr<moeconcept::Cloneable>
ircode::IntArrayStaticValue::_cloneToUniquePtr() const {
	return std::make_unique<IntArrayStaticValue>(*this);
}

ircode::IntArrayStaticValue::IntArrayStaticValue(
	int len, std::vector<IntStaticValue> vi
) : StaticValue(), shape({len}), value(std::move(vi)) {
	StaticValue::uPtrInfo = std::make_unique<IntArrayType>(shape);
	stlextension::vector::PushBackByNumberAndInstance(
		value, len - int(value.size()), ircode::IntStaticValue()
	);
}

ircode::IntArrayStaticValue::IntArrayStaticValue(
	int len, const std::vector<int> & preShape,
	const std::vector<IntArrayStaticValue> & vi
) {
	
	com::Assert(
		len >= (int) vi.size(), "len should less than vi.size().",
		CODEPOS
	);
	shape = {len};
	stlextension::vector::PushBackByIterators(
		shape, preShape.begin(),
		preShape.end());
	for (const auto & fsa : vi
		) {
		if (fsa.shape != preShape) {
			com::Throw("Shape not same.", CODEPOS);
		}
		stlextension::vector::PushBackByIterators(
			value, fsa.value.begin(),
			fsa.value.end());
	}
	int sz = 1;
	for (int x : preShape) { sz *= x; }
	stlextension::vector::PushBackByNumberAndInstance(
		value, (len - int(vi.size())) * sz, IntStaticValue());
	StaticValue::uPtrInfo = std::make_unique<IntArrayType>(shape);
}

std::string ircode::IntArrayStaticValue::toLLVMIR() const {
	std::function<std::string(int, int, int)> fun = [&](
		int from, int to, int dim
	) -> std::string {
		if (dim == int(shape.size())) {
			std::string buf;
			for (int i = from; i < to; ++i
				) {
				buf += value[i].toLLVMIR() + ", ";
			}
			buf.pop_back();
			buf.pop_back();
			return buf;
		}
		std::vector<int> nowShape;
		for (int i = dim; i < int(shape.size()); ++i
			) {
			nowShape.push_back(shape[i]);
		}
		IntArrayType fat(nowShape);
		std::string buf = fat.toLLVMIR() + " [";
		int sz = 1;
		for (int i = dim + 1; i < int(shape.size()); ++i) { sz *= shape[i]; }
		for (int i = 0; i < shape[dim]; ++i
			) {
			buf += fun(from + sz * i, from + sz * (i + 1), dim + 1) + ", ";
		}
		buf.pop_back();
		buf.pop_back();
		return buf + "]";
	};
	int sz = 1;
	for (int x : shape) { sz *= x; }
	return fun(0, sz, 0);
}

std::unique_ptr<ircode::StaticValue>
ircode::IntArrayStaticValue::getValue(const std::vector<int> & ind) const {
	if (ind.size() == shape.size()) {
		size_t idx = 0, stride = 1;
		for (auto itInd = ind.rbegin(), itShape = shape.rbegin();
		     itInd != ind.rend(); ++itInd, ++itShape
			) {
			idx += *itInd * stride;
			stride *= *itShape;
		}
		return com::dynamic_cast_uPtr<StaticValue>(
			value[idx].cloneToUniquePtr());
	} else {
		com::TODO("Maybe do not need this.", CODEPOS);
	}
}

ircode::IntArrayStaticValue::IntArrayStaticValue(
	int len,
	const std::vector<int> & preShape,
	const std::vector<IntArrayStaticValue> & vi,
	const std::vector<IntStaticValue> & rest
) {
	int sz = 1;
	for (int x : preShape) { sz *= x; }
	com::Assert(
		len >= (int) vi.size() + ((int) rest.size() + sz - 1) / sz,
		"len should greater than vi.size()+lower_bound(rest.size()/sz).",
		CODEPOS
	);
	shape = {len};
	stlextension::vector::PushBackByIterators(
		shape, preShape.begin(),
		preShape.end()
	);
	for (const auto & fsa : vi) {
		if (fsa.shape != preShape) {
			com::Throw("Shape not same.", CODEPOS);
		}
		stlextension::vector::PushBackByIterators(
			value, fsa.value.begin(),
			fsa.value.end()
		);
	}
	stlextension::vector::PushBackByIterators(
		value, rest.begin(), rest.end()
	);
	stlextension::vector::PushBackByNumberAndInstance(
		value,
		len * sz - int(vi.size()) * sz - int(rest.size()),
		IntStaticValue()
	);
	StaticValue::uPtrInfo = std::make_unique<IntArrayType>(shape);
}

ircode::IntArrayStaticValue::IntArrayStaticValue(std::vector<int> shape)
	: shape(std::move(shape)) {
	int sz = 1;
	for (auto l : this->shape) {
		sz *= int(l);
	}
	stlextension::vector::PushBackByNumberAndInstance(
		value, sz, ircode::IntStaticValue()
	);
}

std::unique_ptr<moeconcept::Cloneable>
ircode::BoolStaticValue::_cloneToUniquePtr() const {
	return std::make_unique<BoolStaticValue>(*this);
}

ircode::BoolStaticValue::BoolStaticValue(const std::string & literal)
	: StaticValue(BoolType()), value(std::stoi(literal)) {
}

ircode::BoolStaticValue::BoolStaticValue(bool value)
	: StaticValue(BoolType()), value(value) {
}

std::string ircode::BoolStaticValue::toLLVMIR() const {
	return uPtrInfo->toLLVMIR() + " " + intToString(value);
}

std::unique_ptr<ircode::StaticValue>
ircode::BoolStaticValue::getValue(const std::vector<int> &) const {
	return com::dynamic_cast_uPtr<StaticValue>(
		cloneToUniquePtr());
}

std::unique_ptr<ircode::StaticValue> ircode::BoolStaticValue::calc(
	const ircode::StaticValue & oVal, const std::string & op
) const {
	std::unique_ptr<ircode::StaticValue> upSVRes;
	switch (oVal.getTypeInfo().type) {
		case TypeInfo::Type::Float_t: {
			upSVRes = calcOfFloat(
				float(value),
				dynamic_cast<const ircode::FloatStaticValue &>(oVal).value, op
			);
			break;
		}
		case TypeInfo::Type::Int_t: {
			upSVRes = calcOfInt(
				value,
				dynamic_cast<const ircode::IntStaticValue &>(oVal).value, op
			);
			break;
		}
		case TypeInfo::Type::Bool_t: {
			upSVRes = calcOfBool(
				value,
				dynamic_cast<const ircode::BoolStaticValue &>(oVal).value, op
			);
			break;
		}
		default: {
			com::Throw("Type of oVal should be Int_t/Float_t/Bool_t.", CODEPOS);
		}
	}
	return upSVRes;
}

std::unique_ptr<ircode::StaticValue>
ircode::BoolStaticValue::calc(const std::string & op) const {
	return calcOfBool(false, value, op);
}

std::unique_ptr<ircode::StaticValue>
ircode::BoolStaticValue::convertTo(const ircode::TypeInfo & toType) {
	switch (toType.type) {
		case TypeInfo::Type::Bool_t: {
			return std::make_unique<ircode::BoolStaticValue>(bool(value));
		}
		case TypeInfo::Type::Int_t: {
			return std::make_unique<ircode::IntStaticValue>(int(value));
		}
		case TypeInfo::Type::Float_t: {
			return std::make_unique<ircode::FloatStaticValue>(float(value));
		}
		default: {
			com::Throw("Conversion failed since toType unsupported.", CODEPOS);
		}
	}
}
