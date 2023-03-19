const assert = require('assert')
const zkJS = require('../zkJS/build/Release/zkAppUtilsJS.node');

const ZymbitKeyring = require('../src/eth-zymbit-keyring')
const zk = new zkJS.zkObj()

describe('ZymbitKeyring', function () {
    const walletName = "TestWallet"
    const starting_slots = [...zk.getAllocSlotsList(false)].filter(slot => slot > 15)
    const new_wallet = zk.genWalletMasterSeedWithBIP39("secp256k1", walletName)
    console.log(new_wallet)
    const opts = {

    }
    const keyring = new ZymbitKeyring()

    describe('constructor', function(done) {
        it('constructs', function async (done) {
            assert.equal(typeof obj, 'object')
            done()
        })
    })

    describe('Keyring.type', function() {
        it('is a class property that returns the type string.', function() {
            const type = ZymbitKeyring.type
            assert.equal(type, 'Zymbit Hardware Wallet')
        })
    })


})

