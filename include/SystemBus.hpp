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

#ifndef __SYSTEMBUS__
#define __SYSTEMBUS__

#include <stdint.h>
#include <vector>

#include "SystemBusDevice.hpp"

#define PPU_APU_REGS_SIZE                0x200
#define DSP_SUPERFX_REGS_SIZE           0x1000
#define OLD_STYLE_JOYPAD_REGS_SIZE       0x100
#define DMA_PPU2_REGS_SIZE               0x300
#define CARTRIDGE_SRAM_SIZE             0x2000

class SystemBus {
    public:
        SystemBus();
        ~SystemBus();

        void registerDevice(SystemBusDevice *);
        void storeByte(const Address&, uint8_t);
        void storeTwoBytes(const Address&, uint16_t);
        uint8_t readByte(const Address&);
        uint16_t readTwoBytes(const Address&);
        Address readAddressAt(const Address&);

    private:

        std::vector<SystemBusDevice *> mDevices;

        uint8_t *mPpuApuRegs;
        uint8_t *mDspSuperFxRegs;
        uint8_t *mOldStyleJoypadRegs;
        uint8_t *mDmaPpu2Regs;
        uint8_t *mCartridgeSRam;
};

#endif
