//===- FuzzerTracePC.h - Internal header for the Fuzzer ---------*- C++ -* ===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
// fuzzer::TracePC
//===----------------------------------------------------------------------===//

#ifndef LLVM_FUZZER_TRACE_PC
#define LLVM_FUZZER_TRACE_PC

#include "FuzzerDefs.h"
#include "FuzzerValueBitMap.h"

namespace fuzzer {

class TracePC {
 public:
  void HandleTrace(uint32_t *guard, uintptr_t PC);
  void HandleInit(uint32_t *start, uint32_t *stop);
  void HandleCallerCallee(uintptr_t Caller, uintptr_t Callee);
  void HandleValueProfile(size_t Value) { ValueProfileMap.AddValue(Value); }
  size_t GetTotalPCCoverage() { return TotalPCCoverage; }
  void ResetTotalPCCoverage() { TotalPCCoverage = 0; }
  void SetUseCounters(bool UC) { UseCounters = UC; }
  void SetUseValueProfile(bool VP) { UseValueProfile = VP; }
  bool UpdateCounterMap(ValueBitMap *MaxCounterMap) {
    return MaxCounterMap->MergeFrom(CounterMap);
  }
  bool UpdateValueProfileMap(ValueBitMap *MaxValueProfileMap) {
    return UseValueProfile && MaxValueProfileMap->MergeFrom(ValueProfileMap);
  }
  void FinalizeTrace();

  size_t GetNewPCIDs(uintptr_t **NewPCIDsPtr) {
    *NewPCIDsPtr = NewPCIDs;
    return Min(kMaxNewPCIDs, NumNewPCIDs);
  }

  uintptr_t GetPCbyPCID(uintptr_t PCID) { return PCs[PCID]; }

  void ResetMaps() {
    NumNewPCIDs = 0;
    CounterMap.Reset();
    ValueProfileMap.Reset();
    memset(Counters, 0, sizeof(Counters));
  }

  void UpdateFeatureSet(size_t CurrentElementIdx, size_t CurrentElementSize);
  void PrintFeatureSet();

  void ResetGuards();

  void PrintModuleInfo();

  void PrintCoverage();

private:
  bool UseCounters = false;
  bool UseValueProfile = false;
  size_t TotalPCCoverage = 0;

  static const size_t kMaxNewPCIDs = 1024;
  uintptr_t NewPCIDs[kMaxNewPCIDs];
  size_t NumNewPCIDs = 0;
  void AddNewPCID(uintptr_t PCID) {
    NewPCIDs[(NumNewPCIDs++) % kMaxNewPCIDs] = PCID;
  }

  struct Module {
    uint32_t *Start, *Stop;
  };

  Module Modules[4096];
  size_t NumModules = 0;
  size_t NumGuards = 0;

  static const size_t kNumCounters = 1 << 14;
  alignas(8) uint8_t Counters[kNumCounters];

  static const size_t kNumPCs = 1 << 20;
  uintptr_t PCs[kNumPCs];

  ValueBitMap CounterMap;
  ValueBitMap ValueProfileMap;

  struct Feature {
    size_t Count;
    size_t SmallestElementIdx;
    size_t SmallestElementSize;
  };

  static const size_t kFeatureSetSize = ValueBitMap::kNumberOfItems;
  Feature FeatureSet[kFeatureSetSize];
};

extern TracePC TPC;

}  // namespace fuzzer

#endif  // LLVM_FUZZER_TRACE_PC
