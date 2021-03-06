// RaceDetector.h (Oclgrind)
// Copyright (c) 2013-2015, James Price and Simon McIntosh-Smith,
// University of Bristol. All rights reserved.
//
// This program is provided under a three-clause BSD license. For full
// license terms please see the LICENSE file distributed with this
// source code.

#include "core/Plugin.h"

namespace oclgrind
{
  class RaceDetector : public Plugin
  {
  public:
    RaceDetector(const Context *context);

    virtual void kernelBegin(const KernelInvocation *kernelInvocation) override;
    virtual void kernelEnd(const KernelInvocation *kernelInvocation) override;
    virtual void memoryAllocated(const Memory *memory, size_t address,
                                 size_t size, cl_mem_flags flags) override;
    virtual void memoryAtomicLoad(const Memory *memory,
                                  const WorkItem *workItem,
                                  AtomicOp op,
                                  size_t address, size_t size) override;
    virtual void memoryAtomicStore(const Memory *memory,
                                   const WorkItem *workItem,
                                   AtomicOp op,
                                   size_t address, size_t size) override;
    virtual void memoryDeallocated(const Memory *memory, size_t address);
    virtual void memoryLoad(const Memory *memory, const WorkItem *workItem,
                            size_t address, size_t size) override;
    virtual void memoryLoad(const Memory *memory, const WorkGroup *workGroup,
                            size_t address, size_t size) override;
    virtual void memoryStore(const Memory *memory, const WorkItem *workItem,
                             size_t address, size_t size,
                             const uint8_t *storeData) override;
    virtual void memoryStore(const Memory *memory, const WorkGroup *workGroup,
                             size_t address, size_t size,
                             const uint8_t *storeData) override;
    virtual void workGroupBarrier(const WorkGroup *workGroup,
                                  uint32_t flags) override;

    virtual bool isThreadSafe() const override;

  private:
    struct State
    {
      const llvm::Instruction *instruction;
      size_t workItem;
      size_t workGroup;
      bool canAtomicLoad;
      bool canAtomicStore;
      bool canLoad;
      bool canStore;
      bool wasWorkItem;

      State();
    };

    // Enumeration for types of data-race
    enum DataRaceType
    {
      ReadWriteRace,
      WriteWriteRace
    };

    typedef std::map<
                      std::pair<const Memory*, size_t>,
                      std::pair<State*, size_t>
                    > StateMap;
    StateMap m_state;

    bool m_allowUniformWrites;
    const KernelInvocation *m_kernelInvocation;

    void logRace(DataRaceType type,
                 unsigned int addrSpace,
                 size_t address,
                 size_t lastWorkGroup,
                 size_t lastWorkItem,
                 const llvm::Instruction *lastInstruction) const;
    void registerAtomic(const Memory *memory,
                        const WorkItem *workItem,
                        size_t address, size_t size,
                        bool store);
    void registerLoadStore(const Memory *memory,
                           const WorkItem *workItem,
                           const WorkGroup *workGroup,
                           size_t address, size_t size,
                           const uint8_t *storeData);
    void synchronize(const Memory *memory, bool workGroup);
  };
}
