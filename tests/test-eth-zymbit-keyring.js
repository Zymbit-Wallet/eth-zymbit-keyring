const assert = require('assert')
const ethUtil = require('ethereumjs-util')
const sigUtil = require('eth-sig-util')
const EthereumTx = require('ethereumjs-tx')

const ZymbitKeyring = require('../src/eth-zymbit-keyring')

describe('ZymbitKeyring', function () {
    let keyring
    beforeEach(async function() {
        keyring = new ZymbitKeyring()
    })

    describe('constructor', function(done) {
        it('constructs', function async (done) {
            const obj = new ZymbitKeyring({hdPath: `m/44'/60'/0'/0`})
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

    describe('#type', function() {
        it('returns the correct value', function() {
            const type = keyring.type
            const correct = TrezorKeyring.type
            assert.equal(type, correct)
        })
    })

    describe('#serialize', function() {

    })


})

