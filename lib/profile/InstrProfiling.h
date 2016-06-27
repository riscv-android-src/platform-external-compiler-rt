/*===- InstrProfiling.h- Support library for PGO instrumentation ----------===*\
|*
|*                     The LLVM Compiler Infrastructure
|*
|* This file is distributed under the University of Illinois Open Source
|* License. See LICENSE.TXT for details.
|*
\*===----------------------------------------------------------------------===*/

#ifndef PROFILE_INSTRPROFILING_H_
#define PROFILE_INSTRPROFILING_H_

#include "InstrProfilingPort.h"
#include "InstrProfData.inc"

enum ValueKind {
#define VALUE_PROF_KIND(Enumerator, Value) Enumerator = Value,
#include "InstrProfData.inc"
};

typedef void *IntPtrT;
typedef struct COMPILER_RT_ALIGNAS(INSTR_PROF_DATA_ALIGNMENT)
    __llvm_profile_data {
#define INSTR_PROF_DATA(Type, LLVMType, Name, Initializer) Type Name;
#include "InstrProfData.inc"
} __llvm_profile_data;

typedef struct __llvm_profile_header {
#define INSTR_PROF_RAW_HEADER(Type, Name, Initializer) Type Name;
#include "InstrProfData.inc"
} __llvm_profile_header;

typedef struct ValueProfNode * PtrToNodeT;
typedef struct ValueProfNode {
#define INSTR_PROF_VALUE_NODE(Type, LLVMType, Name, Initializer) Type Name;
#include "InstrProfData.inc"
} ValueProfNode;

/*!
 * \brief Get number of bytes necessary to pad the argument to eight
 * byte boundary.
 */
uint8_t __llvm_profile_get_num_padding_bytes(uint64_t SizeInBytes);

/*!
 * \brief Get required size for profile buffer.
 */
uint64_t __llvm_profile_get_size_for_buffer(void);

/*!
 * \brief Write instrumentation data to the given buffer.
 *
 * \pre \c Buffer is the start of a buffer at least as big as \a
 * __llvm_profile_get_size_for_buffer().
 */
int __llvm_profile_write_buffer(char *Buffer);

const __llvm_profile_data *__llvm_profile_begin_data(void);
const __llvm_profile_data *__llvm_profile_end_data(void);
const char *__llvm_profile_begin_names(void);
const char *__llvm_profile_end_names(void);
uint64_t *__llvm_profile_begin_counters(void);
uint64_t *__llvm_profile_end_counters(void);
ValueProfNode *__llvm_profile_begin_vnodes();
ValueProfNode *__llvm_profile_end_vnodes();

/*!
 * \brief Clear profile counters to zero.
 *
 */
void __llvm_profile_reset_counters(void);

/*!
 * \brief Merge profile data from buffer.
 *
 * Read profile data form buffer \p Profile  and merge with
 * in-process profile counters. The client is expected to
 * have checked or already knows the profile data in the
 * buffer matches the in-process counter structure before
 * calling it.
 */
void __llvm_profile_merge_from_buffer(const char *Profile, uint64_t Size);

/*! \brief Check if profile in buffer matches the current binary.
 *
 *  Returns 0 (success) if the profile data in buffer \p Profile with size
 *  \p Size was generated by the same binary and therefore matches
 *  structurally the in-process counters. If the profile data in buffer is
 *  not compatible, the interface returns 1 (failure).
 */
int __llvm_profile_check_compatibility(const char *Profile,
                                       uint64_t Size);

/*!
 * \brief Counts the number of times a target value is seen.
 *
 * Records the target value for the CounterIndex if not seen before. Otherwise,
 * increments the counter associated w/ the target value.
 * void __llvm_profile_instrument_target(uint64_t TargetValue, void *Data,
 *                                       uint32_t CounterIndex);
 */
void INSTR_PROF_VALUE_PROF_FUNC(
#define VALUE_PROF_FUNC_PARAM(ArgType, ArgName, ArgLLVMType) ArgType ArgName
#include "InstrProfData.inc"
    );

/*!
 * \brief Write instrumentation data to the current file.
 *
 * Writes to the file with the last name given to \a *
 * __llvm_profile_set_filename(),
 * or if it hasn't been called, the \c LLVM_PROFILE_FILE environment variable,
 * or if that's not set, the last name given to
 * \a __llvm_profile_override_default_filename(), or if that's not set,
 * \c "default.profraw".
 */
int __llvm_profile_write_file(void);

/*!
 * \brief Set the filename for writing instrumentation data.
 *
 * Sets the filename to be used for subsequent calls to
 * \a __llvm_profile_write_file().
 *
 * \c Name is not copied, so it must remain valid.  Passing NULL resets the
 * filename logic to the default behaviour.
 */
void __llvm_profile_set_filename(const char *Name);

/*!
 * \brief Set the filename for writing instrumentation data, unless the
 * \c LLVM_PROFILE_FILE environment variable was set.
 *
 * Unless overridden, sets the filename to be used for subsequent calls to
 * \a __llvm_profile_write_file().
 *
 * \c Name is not copied, so it must remain valid.  Passing NULL resets the
 * filename logic to the default behaviour (unless the \c LLVM_PROFILE_FILE
 * was set in which case it has no effect).
 */
void __llvm_profile_override_default_filename(const char *Name);

/*! \brief Register to write instrumentation data to file at exit. */
int __llvm_profile_register_write_file_atexit(void);

/*! \brief Initialize file handling. */
void __llvm_profile_initialize_file(void);

/*! \brief Get the magic token for the file format. */
uint64_t __llvm_profile_get_magic(void);

/*! \brief Get the version of the file format. */
uint64_t __llvm_profile_get_version(void);

/*! \brief Get the number of entries in the profile data section. */
uint64_t __llvm_profile_get_data_size(const __llvm_profile_data *Begin,
                                      const __llvm_profile_data *End);

/*!
 * This variable is defined in InstrProfilingRuntime.cc as a hidden
 * symbol. Its main purpose is to enable profile runtime user to
 * bypass runtime initialization code -- if the client code explicitly
 * define this variable, then InstProfileRuntime.o won't be linked in.
 * Note that this variable's visibility needs to be hidden so that the
 * definition of this variable in an instrumented shared library won't
 * affect runtime initialization decision of the main program.
 */
COMPILER_RT_VISIBILITY extern int __llvm_profile_runtime;

/*!
 * This variable is defined in InstrProfiling.c. Its main purpose is to
 * encode the raw profile version value and other format related information
 * such as whether the profile is from IR based instrumentation. The variable
 * is defined as weak so that compiler can emit an overriding definition
 * depending on user option.  Since we don't support mixing FE and IR based
 * data in the same raw profile data file (in other words, shared libs and
 * main program are expected to be instrumented in the same way), there is
 * no need for this variable to be hidden.
 */
extern uint64_t __llvm_profile_raw_version;

#endif /* PROFILE_INSTRPROFILING_H_ */
