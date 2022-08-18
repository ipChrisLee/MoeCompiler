#include "backend/backendPass.hpp"
#include <stack>


namespace pass {

int FuncInfo::run(ircode::InstrParallelCopy * pInstrCopy) {
	for (auto [from, to, _]: pInstrCopy->copies) {
		markOperand(from);
		markOperand(to);
	}
	return 0;
}

std::string FuncInfo::toASM(ircode::InstrParallelCopy * pInstrCopy) {
	auto res = std::string();
	res += toASM_Copy_Int(pInstrCopy);
	res += toASM_Copy_Float(pInstrCopy);
	return res;
}

std::string FuncInfo::toASM_Copy_Float(ircode::InstrParallelCopy * pInstrCopy) {
	auto res = std::string();
	struct stat {
		backend::SId where = backend::SId::err;
		int32_t offset = INT_MIN;

		bool operator<(const stat & s) const {
			if (int(where) != int(s.where)) {
				return int(where) < int(s.where);
			} else {
				return offset < s.offset;
			}
		}
	};
	auto statsId = std::map<stat, int>();
	auto idToStats = std::vector<stat>();
	auto add_stat = [&statsId, &idToStats](backend::SId sid, int32_t offset) {
		stat s = {sid, offset};
		if (statsId.count(s)) {
			return statsId[s];
		} else {
			statsId[s] = int(idToStats.size());
			idToStats.emplace_back(s);
			return int(idToStats.size()) - 1;
		}
	};
	auto G = std::map<int, std::vector<int>>();
	auto din = std::map<int, int>();
	auto dout = std::map<int, int>();
	auto fromImmToStat = std::vector<std::pair<float, stat>>();
	for (auto [pAddrFrom, pAddrTo, _]: pInstrCopy->copies) {
		if (!com::enum_fun::in(
			pAddrTo->getType().type, {sup::Type::Float_t}
		)) { continue; }
		auto statIdFrom = 0, statIdTo = 0;
		auto fromImm = false;
		auto imm = 0.0f;
		switch (pAddrFrom->addrType) {
			case ircode::AddrType::StaticValue: {
				auto * pSVFrom = dynamic_cast<ircode::AddrStaticValue *>(pAddrFrom);
				imm = dynamic_cast<const sup::FloatStaticValue &>(
					pSVFrom->getStaticValue()
				).value;
				fromImm = true;
				break;
			}
			case ircode::AddrType::Var: {
				auto * pVarFrom = dynamic_cast<ircode::AddrVariable *>(pAddrFrom);
				auto * pVRegS = convertFloatVariable(pVarFrom);
				statIdFrom = add_stat(pVRegS->sid, pVRegS->offset);
				break;
			}
			default:com::Throw("", CODEPOS);
		}
		switch (pAddrTo->addrType) {
			case ircode::AddrType::Var: {
				auto * pVarTo = dynamic_cast<ircode::AddrVariable *>(pAddrTo);
				auto * pVRegS = convertFloatVariable(pVarTo);
				statIdTo = add_stat(pVRegS->sid, pVRegS->offset);
				break;
			}
			default:com::Throw("", CODEPOS);
		}
		if (fromImm) {
			fromImmToStat.emplace_back(imm, idToStats[statIdTo]);
		} else {
			G[statIdFrom].emplace_back(statIdTo);
			din[statIdTo] += 1;
			dout[statIdFrom] += 1;
		}
	}
	auto moveData = [&res](stat statFrom, stat statTo) {
		if (backend::isGPR(statFrom.where)) {
			if (backend::isGPR(statTo.where)) { //  from reg to reg
				res += backend::toASM("vmov", statTo.where, statFrom.where);
			} else if (statTo.where == backend::SId::stk) {  //  from reg to stk
				genASMSaveFromSRegToOffset(
					res, statFrom.where, statTo.offset, backend::RId::lhs
				);
			} else { com::Throw("", CODEPOS); }
		} else if (statFrom.where == backend::SId::stk) {
			if (backend::isGPR(statTo.where)) { //  from stk to reg
				genASMDerefStkPtrToSReg(res, statFrom.offset, statTo.where, backend::RId::lhs);
			} else if (statTo.where == backend::SId::stk) {  //  from stk to stk
				genASMDerefStkPtr(res, statFrom.offset, backend::RId::lhs);
				genASMSaveFromRRegToOffset(
					res, backend::RId::lhs, statTo.offset, backend::RId::rhs
				);
			} else { com::Throw("", CODEPOS); }
		}
	};
	auto inLoop = std::map<int, bool>();
	auto vis = std::map<int, bool>();
	std::function<int(int)> dfsCheckLoop = [&dfsCheckLoop, &inLoop, &G, &vis](int u) -> int {
		auto d = -1;
		vis[u] = true;
		for (auto v: G[u]) {
			if (!vis[v]) {
				auto dd = dfsCheckLoop(v);
				if (dd != -1) {
					d = dd;
					inLoop[u] = true;
				}
			} else {
				d = v;
			}
		}
		if (d == u) { d = -1; }
		return d;
	};
	for (const auto & [s, statIdSrc]: statsId) {
		if (!vis[statIdSrc]) {
			dfsCheckLoop(statIdSrc);
		}
	}
	vis.clear();
	for (const auto & [s, statIdSrc]: statsId) {
		if (vis[statIdSrc]) { continue; }
		if (G[statIdSrc].size() == 1 && G[statIdSrc][0] == statIdSrc) { continue; }
		auto st = std::deque<std::pair<int, int>>();
		if (din[statIdSrc] == 0 && !inLoop[statIdSrc]) { //  statIdSrc is front of a chain.
			auto q = std::queue<int>();
			q.push(statIdSrc);
			while (!q.empty()) {
				auto u = q.front();
				q.pop();
				com::Assert(!vis[u], "", CODEPOS);
				vis[u] = true;
				for (auto v: G[u]) {
					com::Assert(!vis[v], "", CODEPOS);
					st.emplace_front(u, v);
					q.push(v);
				}
			}
		} else if (din[statIdSrc] == 1 && inLoop[statIdSrc]) {
			//  statIdSrc is in loop, and may be front of chain
			auto q = std::queue<int>();
			q.push(statIdSrc);
			while (!q.empty()) {
				auto u = q.front();
				q.pop();
				com::Assert(!vis[u], "", CODEPOS);
				vis[u] = true;
				for (auto v: G[u]) {
					if (v == statIdSrc) {
						st.emplace_front(u, -1);
						continue;
					}
					com::Assert(!vis[v], "", CODEPOS);
					st.emplace_front(u, v);
					q.push(v);
				}
			}
			st.emplace_back(-1, statIdSrc);
		} else if (din[statIdSrc] == 1 && !inLoop[statIdSrc]) {
			//  in chain, do nothing
		} else { com::Throw("", CODEPOS); }
		while (!st.empty()) {
			auto [from, to] = st.front();
			st.pop_front();
			if (from == -1) {
				//  from lhs to dest
				moveData({backend::SId::stk, -4}, idToStats[to]);
			} else if (to == -1) {
				//  from reg to lhs
				moveData(idToStats[from], {backend::SId::stk, -4});
			} else {
				moveData(idToStats[from], idToStats[to]);
			}
		}
	}
	for (auto [imm, s]: fromImmToStat) {
		if (backend::isGPR(s.where)) {
			genASMLoadFloat(res, imm, s.where, backend::RId::lhs);
		} else if (s.where == backend::SId::stk) {
			genASMLoadFloatToRReg(res, imm, backend::RId::lhs);
			genASMSaveFromRRegToOffset(res, backend::RId::lhs, s.offset, backend::RId::rhs);
		}
	}
	return res;
}

std::string FuncInfo::toASM_Copy_Int(ircode::InstrParallelCopy * pInstrCopy) {
	auto res = std::string();
	struct stat {
		backend::RId where = backend::RId::err;
		int32_t offset = INT_MIN;

		bool operator<(const stat & s) const {
			if (int(where) != int(s.where)) {
				return int(where) < int(s.where);
			} else {
				return offset < s.offset;
			}
		}
	};
	auto statsId = std::map<stat, int>();
	auto idToStats = std::vector<stat>();
	auto add_stat = [&statsId, &idToStats](backend::RId rid, int32_t offset) {
		stat s = {rid, offset};
		if (statsId.count(s)) {
			return statsId[s];
		} else {
			statsId[s] = int(idToStats.size());
			idToStats.emplace_back(s);
			return int(idToStats.size()) - 1;
		}
	};
	auto G = std::map<int, std::vector<int>>();
	auto din = std::map<int, int>();
	auto dout = std::map<int, int>();
	auto fromImmToStat = std::vector<std::pair<int32_t, stat>>();
	for (auto [pAddrFrom, pAddrTo, _]: pInstrCopy->copies) {
		if (!com::enum_fun::in(
			pAddrTo->getType().type, {sup::Type::Int_t, sup::Type::Pointer_t}
		)) { continue; }
		auto statIdFrom = 0, statIdTo = 0;
		auto fromImm = false;
		auto imm = 0;
		switch (pAddrFrom->addrType) {
			case ircode::AddrType::StaticValue: {
				auto * pSVFrom = dynamic_cast<ircode::AddrStaticValue *>(pAddrFrom);
				imm = dynamic_cast<const sup::IntStaticValue &>(
					pSVFrom->getStaticValue()
				).value;
				fromImm = true;
				break;
			}
			case ircode::AddrType::Var: {
				auto * pVarFrom = dynamic_cast<ircode::AddrVariable *>(pAddrFrom);
				auto * pVRegR = convertIntVariable(pVarFrom);
				statIdFrom = add_stat(pVRegR->rid, pVRegR->offset);
				break;
			}
			default:com::Throw("", CODEPOS);
		}
		switch (pAddrTo->addrType) {
			case ircode::AddrType::Var: {
				auto * pVarTo = dynamic_cast<ircode::AddrVariable *>(pAddrTo);
				auto * pVRegR = convertIntVariable(pVarTo);
				statIdTo = add_stat(pVRegR->rid, pVRegR->offset);
				break;
			}
			default:com::Throw("", CODEPOS);
		}
		if (fromImm) {
			fromImmToStat.emplace_back(imm, idToStats[statIdTo]);
		} else {
			G[statIdFrom].emplace_back(statIdTo);
			din[statIdTo] += 1;
			dout[statIdFrom] += 1;
		}
	}
	auto moveData = [&res](stat statFrom, stat statTo) {
		//  statTo.where can be rhs
		if (backend::isGPR(statFrom.where)) {
			if (backend::isGPR(statTo.where)) { //  from reg to reg
				res += backend::toASM("mov", statTo.where, statFrom.where);
			} else if (statTo.where == backend::RId::stk) {  //  from reg to stk
				genASMSaveFromRRegToOffset(
					res, statFrom.where, statTo.offset, backend::RId::lhs
				);
			} else { com::Throw("", CODEPOS); }
		} else if (statFrom.where == backend::RId::stk) {
			if (backend::isGPR(statTo.where)) { //  from stk to reg
				genASMDerefStkPtr(res, statFrom.offset, statTo.where);
			} else if (statTo.where == backend::RId::stk) {  //  from stk to stk
				genASMDerefStkPtr(res, statFrom.offset, backend::RId::lhs);
				genASMSaveFromRRegToOffset(
					res, backend::RId::lhs, statTo.offset, backend::RId::rhs
				);
			} else { com::Throw("", CODEPOS); }
		}
	};
	auto inLoop = std::map<int, bool>();
	auto vis = std::map<int, bool>();
	std::function<int(int)> dfsCheckLoop = [&dfsCheckLoop, &inLoop, &G, &vis](int u) -> int {
		auto d = -1;
		vis[u] = true;
		for (auto v: G[u]) {
			if (!vis[v]) {
				auto dd = dfsCheckLoop(v);
				if (dd != -1) {
					d = dd;
					inLoop[u] = true;
				}
			} else {
				d = v;
			}
		}
		if (d == u) { d = -1; }
		return d;
	};
	for (const auto & [s, statIdSrc]: statsId) {
		if (!vis[statIdSrc]) {
			dfsCheckLoop(statIdSrc);
		}
	}
	vis.clear();
	for (const auto & [s, statIdSrc]: statsId) {
		if (vis[statIdSrc]) { continue; }
		if (G[statIdSrc].size() == 1 && G[statIdSrc][0] == statIdSrc) { continue; }
		auto st = std::deque<std::pair<int, int>>();
		if (din[statIdSrc] == 0 && !inLoop[statIdSrc]) { //  statIdSrc is front of a chain.
			auto q = std::queue<int>();
			q.push(statIdSrc);
			while (!q.empty()) {
				auto u = q.front();
				q.pop();
				com::Assert(!vis[u], "", CODEPOS);
				vis[u] = true;
				for (auto v: G[u]) {
					com::Assert(!vis[v], "", CODEPOS);
					st.emplace_front(u, v);
					q.push(v);
				}
			}
		} else if (din[statIdSrc] == 1 && inLoop[statIdSrc]) {
			//  statIdSrc is in loop, and may be front of chain
			auto q = std::queue<int>();
			q.push(statIdSrc);
			while (!q.empty()) {
				auto u = q.front();
				q.pop();
				com::Assert(!vis[u], "", CODEPOS);
				vis[u] = true;
				for (auto v: G[u]) {
					if (v == statIdSrc) {
						st.emplace_front(u, -1);
						continue;
					}
					com::Assert(!vis[v], "", CODEPOS);
					st.emplace_front(u, v);
					q.push(v);
				}
			}
			st.emplace_back(-1, statIdSrc);
		} else if (din[statIdSrc] == 1 && !inLoop[statIdSrc]) {
			//  in chain, do nothing
		} else { com::Throw("", CODEPOS); }
		while (!st.empty()) {
			auto [from, to] = st.front();
			st.pop_front();
			if (from == -1) {
				//  from lhs to dest
				moveData({backend::RId::stk, -4}, idToStats[to]);
			} else if (to == -1) {
				//  from reg to lhs
				moveData(idToStats[from], {backend::RId::stk, -4});
			} else {
				moveData(idToStats[from], idToStats[to]);
			}
		}
	}
	for (auto [imm, s]: fromImmToStat) {
		if (backend::isGPR(s.where)) {
			genASMLoadInt(res, imm, s.where);
		} else if (s.where == backend::RId::stk) {
			genASMLoadInt(res, imm, backend::RId::lhs);
			genASMSaveFromRRegToOffset(
				res, backend::RId::lhs, s.offset, backend::RId::rhs
			);
		}
	}
	return res;
}

int FuncInfo::run(ircode::InstrParaMov * pInstrParaMov) {
	markOperand(pInstrParaMov->from);
	markOperand(pInstrParaMov->to);
	return 0;
}

std::string FuncInfo::toASM(ircode::InstrParaMov * pInstrParaMov) {
	auto res = std::string();
	auto * pFrom = pInstrParaMov->from;
	auto * pTo = pInstrParaMov->to;
	switch (pTo->getType().type) {
		case sup::Type::Int_t:
		case sup::Type::Pointer_t: {
			auto * pVRegRFrom = convertThisIntArg(pFrom);
			auto * pVRegRTo = convertIntVariable(pTo);
			genASMSaveFromVRegRToVRegR(
				res, pVRegRFrom, pVRegRTo, backend::RId::lhs, backend::RId::rhs
			);
			break;
		}
		case sup::Type::Float_t: {
			auto * pVRegSFrom = convertThisFloatArg(pFrom);
			auto * pVRegSTo = convertFloatVariable(pTo);
			genASMSaveFromVRegSToVRegS(
				res, pVRegSFrom, pVRegSTo, backend::RId::lhs, backend::RId::rhs
			);
			break;
		}
		default:com::Throw("", CODEPOS);
	}
	return res;
}

int FuncInfo::run(ircode::InstrMarkVars * pInstrMark) {
	for (auto * p: pInstrMark->vecPVar) {
		markOperand(p);
	}
	return 0;
}

std::string FuncInfo::toASM(ircode::InstrMarkVars * pInstrMark) {
	return "";
}

}