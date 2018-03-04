/*
 * This file is part of the 65816 Emulator Library.
 * Copyright (c) 2018 Francesco Rigoni.
 *
 * https://github.com/FrancescoRigoni/Lib65816
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Cpu65816.hpp"

#include <cmath>

#ifdef EMU_65C02
#define LOG_TAG "Cpu65C02"
#else
#define LOG_TAG (mCpuStatus.emulationFlag() ? "Cpu6502" : "Cpu65816")
#endif

Cpu65816::Cpu65816(SystemBus &systemBus, EmulationModeInterrupts *emulationInterrupts, NativeModeInterrupts *nativeInterrupts) :
            mSystemBus(systemBus),
            mEmulationInterrupts(emulationInterrupts),
            mNativeInterrupts(nativeInterrupts),
            mStack(&mSystemBus) {
}

/**
 * Resets the cpu to its initial state.
 * */
void Cpu65816::reset() {
    setRESPin(true);
    mCpuStatus.setEmulationFlag();
    mCpuStatus.setAccumulatorWidthFlag();
    mCpuStatus.setIndexWidthFlag();
    mX &= 0xFF;
    mY &= 0xFF;
    mD = 0x0;
    mStack = Stack(&mSystemBus);
    mProgramAddress = Address(0x00, mEmulationInterrupts->reset);
}

void Cpu65816::setRESPin(bool value) {
    if (value == false && mPins.RES == true) {
        reset();
    }
    mPins.RES = value;
}

void Cpu65816::setRDYPin(bool value) {
    mPins.RDY = value;
}

bool Cpu65816::executeNextInstruction() {
    if (mPins.RES) {
        return false;
    }

    // Fetch the instruction
    const uint8_t instruction = mSystemBus.readByte(mProgramAddress);
    OpCode opCode = OP_CODE_TABLE[instruction];

    // Execute it
    return opCode.execute(*this);
}

bool Cpu65816::accumulatorIs8BitWide() {
    // Accumulator is always 8 bit in emulation mode.
    if (mCpuStatus.emulationFlag()) return true;
    // Accumulator width set to one means 8 bit accumulator.
    else return mCpuStatus.accumulatorWidthFlag();
}

bool Cpu65816::accumulatorIs16BitWide() {
    return !accumulatorIs8BitWide();
}

bool Cpu65816::indexIs8BitWide() {
    // Index is always 8 bit in emulation mode.
    if (mCpuStatus.emulationFlag()) return true;
    // Index width set to one means 8 bit accumulator.
    else return mCpuStatus.indexWidthFlag();
}

bool Cpu65816::indexIs16BitWide() {
    return !indexIs8BitWide();
}

void Cpu65816::addToCycles(int cycles) {
    mTotalCyclesCounter += cycles;
}

void Cpu65816::subtractFromCycles(int cycles) {
    mTotalCyclesCounter -= cycles;
}

void Cpu65816::addToProgramAddress(int bytes) {
    mProgramAddress.incrementOffsetBy(bytes);
}

void Cpu65816::addToProgramAddressAndCycles(int bytes, int cycles) {
    addToCycles(cycles);
    addToProgramAddress(bytes);
}

uint16_t Cpu65816::indexWithXRegister() {
    return indexIs8BitWide() ? Binary::lower8BitsOf(mX) : mX;
}

uint16_t Cpu65816::indexWithYRegister() {
    return indexIs8BitWide() ? Binary::lower8BitsOf(mY) : mY;
}

void Cpu65816::setProgramAddress(const Address &address) {
    mProgramAddress = address;
}