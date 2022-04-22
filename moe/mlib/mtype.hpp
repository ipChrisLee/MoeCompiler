#pragma once

#include <iostream>
#include <string>

#include "moeconcept.hpp"

namespace mtype{

class quat_t : public moeconcept::Stringable {
  public:
    enum class QValue{ False,True,Undefined,Chaos }qvalue;
    quat_t (QValue qv):qvalue(qv){}

    quat_t():qvalue(QValue::Undefined){}
    quat_t(bool b){
        if(b) qvalue=QValue::True;
        else qvalue=QValue::False;
    }
    quat_t(const quat_t &) = default;
    quat_t & operator = (const quat_t &) = default;
    bool operator == (const quat_t & q){ return qvalue==q.qvalue; }
    quat_t operator && (const quat_t & q) { 
        if(isChaos() || q.isChaos()) return QValue::Chaos;
        if(isUndefined() || q.isUndefined()) return QValue::Undefined;
        if(isTrue() && q.isTrue()) return QValue::True;
        return QValue::False;
    }
    quat_t operator ! () {
        if(isTrue()) return QValue::False;
        if(isFalse()) return QValue::True;
        return *this;
    }
    bool isUndefined() const { return qvalue==QValue::Undefined; }
    bool isTrue() const { return qvalue==QValue::True; }
    bool isFalse() const { return qvalue==QValue::False; }
    bool isChaos() const { return qvalue==QValue::Chaos; }
    virtual std::string toString() const override {
        switch (qvalue) {
            case QValue::False      : return "qFalse";
            case QValue::True       : return "qTrue";
            case QValue::Undefined  : return "qUndefined";
            case QValue::Chaos      : return "qChaos";
        }
    }
    friend std::ostream & operator << (std::ostream & os,const quat_t & q){
        os<<q.toString();return os;
    }
};

extern quat_t qFalse;
extern quat_t qTrue;
extern quat_t qUndefined;
extern quat_t qChaos;

}
