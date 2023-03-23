const assert = require('assert')
const zkJS = require('../zkJS/build/Release/zkAppUtilsJS.node');

const ZymbitKeyring = require('../src/eth-zymbit-keyring')
const zk = new zkJS.zkObj()

const getStartingSlots = () => {
    const startingSlots = []
    const slotsBuffer = zk.getAllocSlotsList(false)
    for (let i = 0; i < slotsBuffer.length / 4; i++) {
      const slot = slotsBuffer.readInt32LE(4 * i)
      if (slot > 15) startingSlots.push(slot)
    }
    return startingSlots
}

const removeCreatedKeys = startingSlots => {
    const slotsBuffer = zk.getAllocSlotsList(false)
    for (let i = 0; i < slotsBuffer.length / 4; i++) {
      const slot = slotsBuffer.readInt32LE(4 * i)
      if (slot > 15 && !startingSlots.includes(slot)) zk.removeKey(slot, false)
    }
}

describe('ZymbitKeyring', function () {
    //Get Starting Slots
    const startingSlots = getStartingSlots()

    // Create mnemonic and initialize keyring
    const wallet_name = "TestWallet"
    const { slot, mnemonic } = zk.genWalletMasterSeedWithBIP39("secp256k1", wallet_name, "", "")
    const opts = {
        wallet_name,
        master_slot: slot
    }
    const keyring = new ZymbitKeyring(opts)

    describe('constructor', function(done) {
        it('constructs', function () {
            assert.equal(typeof keyring, 'object')
        })
    })

    describe('Keyring.type', function() {
        it('is a class property that returns the correct type', function() {
            const type = ZymbitKeyring.type
            assert.equal(type, 'Zymbit Hardware Wallet')
        })
    })

    removeCreatedKeys(startingSlots)

})

