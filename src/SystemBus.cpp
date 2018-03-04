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

#include <cmath>
#include "SystemBus.hpp"
#include "Log.hpp"

#define LOG_TAG "SystemBus"

SystemBus::SystemBus() {
    mPpuApuRegs = new uint8_t[PPU_APU_REGS_SIZE];
    mDspSuperFxRegs = new uint8_t[DSP_SUPERFX_REGS_SIZE];
    mOldStyleJoypadRegs = new uint8_t[OLD_STYLE_JOYPAD_REGS_SIZE];
    mDmaPpu2Regs = new uint8_t[DMA_PPU2_REGS_SIZE];
    // TODO: allocate only if present on cartridge
    mCartridgeSRam = new uint8_t[CARTRIDGE_SRAM_SIZE];

}

SystemBus::~SystemBus() {
    delete[] mPpuApuRegs;
    delete[] mDspSuperFxRegs;
    delete[] mOldStyleJoypadRegs;
    delete[] mDmaPpu2Regs;
    delete[] mCartridgeSRam;
}

void SystemBus::registerDevice(SystemBusDevice *device) {
    mDevices.push_back(device);
}

void SystemBus::storeByte(const Address &address, uint8_t value) {
    for (SystemBusDevice *device : mDevices) {
        Address decodedAddress;
        if (device->decodeAddress(address, decodedAddress)) {
            device->storeByte(decodedAddress, value);
            return;
        }
    }
}

void SystemBus::storeTwoBytes(const Address &address, uint16_t value) {
    for (SystemBusDevice *device : mDevices) {
        Address decodedAddress;
        if (device->decodeAddress(address, decodedAddress)) {
            uint8_t leastSignificantByte = (uint8_t)(value & 0xFF);
            uint8_t mostSignificantByte = (uint8_t)((value & 0xFF00) >> 8);
            device->storeByte(decodedAddress, leastSignificantByte);
            decodedAddress.incrementOffsetBy(1);
            device->storeByte(decodedAddress, mostSignificantByte);
            return;
        }
    }
}

uint8_t SystemBus::readByte(const Address &address) {
    for (SystemBusDevice *device : mDevices) {
        Address decodedAddress;
        if (device->decodeAddress(address, decodedAddress)) {
            return device->readByte(decodedAddress);
        }
    }
    return 0;
}

uint16_t SystemBus::readTwoBytes(const Address &address) {
    for (SystemBusDevice *device : mDevices) {
        Address decodedAddress;
        if (device->decodeAddress(address, decodedAddress)) {
            uint8_t leastSignificantByte = device->readByte(decodedAddress);
            decodedAddress.incrementOffsetBy(sizeof(uint8_t));
            uint8_t mostSignificantByte = device->readByte(decodedAddress);
            uint16_t value = ((uint16_t)mostSignificantByte << 8) | leastSignificantByte;
            return value;
        }
    }
    return 0;
}

Address SystemBus::readAddressAt(const Address &address) {
    Address decodedAddress { 0x00, 0x0000 };
    for (SystemBusDevice *device : mDevices) {
        if (device->decodeAddress(address, decodedAddress)) {
            // Read offset
            uint8_t leastSignificantByte = device->readByte(decodedAddress);
            decodedAddress.incrementOffsetBy(sizeof(uint8_t));
            uint8_t mostSignificantByte = device->readByte(decodedAddress);
            uint16_t offset = ((uint16_t)mostSignificantByte << 8) | leastSignificantByte;
            // Read bank
            decodedAddress.incrementOffsetBy(sizeof(uint8_t));
            uint8_t bank = device->readByte(decodedAddress);
            return Address(bank, offset);
        }
    }
    return decodedAddress;
}
