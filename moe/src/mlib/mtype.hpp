#pragma once

#include <iostream>
#include <string>
#include <cstdint>

#include "moeconcept.hpp"

namespace mtype {

class quat_t : public moeconcept::Stringable {
  public:
	
	enum class QValue {
		False, True, Undefined, Chaos
	} qvalue;
	
	quat_t(QValue qv) : qvalue(qv) { } // NOLINT
	
	quat_t() : qvalue(QValue::Undefined) { }
	
	explicit quat_t(bool b) {
		if (b) qvalue = QValue::True;
		else qvalue = QValue::False;
	}
	
	quat_t(const quat_t &) = default;
	
	quat_t & operator =(const quat_t &) = default;
	
	bool operator ==(const quat_t & q) const { return qvalue == q.qvalue; }
	
	quat_t operator &&(const quat_t & q) const {
		if (isChaos() || q.isChaos()) return QValue::Chaos;
		if (isUndefined() || q.isUndefined()) return QValue::Undefined;
		if (isTrue() && q.isTrue()) return QValue::True;
		return QValue::False;
	}
	
	quat_t operator !() {
		if (isTrue()) return QValue::False;
		if (isFalse()) return QValue::True;
		return *this;
	}
	
	[[nodiscard]] bool isUndefined() const { return qvalue == QValue::Undefined; }
	
	[[nodiscard]] bool isTrue() const { return qvalue == QValue::True; }
	
	[[nodiscard]] bool isFalse() const { return qvalue == QValue::False; }
	
	[[nodiscard]] bool isChaos() const { return qvalue == QValue::Chaos; }
	
	[[nodiscard]] std::string toString() const override {
		switch (qvalue) {
			case QValue::False      :
				return "qFalse";
			case QValue::True       :
				return "qTrue";
			case QValue::Undefined  :
				return "qUndefined";
			case QValue::Chaos      :
				return "qChaos";
		}
	}
	
	friend std::ostream & operator <<(std::ostream & os, const quat_t & q) {
		os << q.toString();
		return os;
	}
};

extern quat_t qFalse;
extern quat_t qTrue;
extern quat_t qUndefined;
extern quat_t qChaos;

}

typedef int64_t i64;
typedef int32_t i32;
typedef bool i1;
