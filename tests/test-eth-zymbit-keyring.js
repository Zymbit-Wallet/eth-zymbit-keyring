const assert = require('assert')
const zkJS = require('../zkJS/build/Release/zkAppUtilsJS.node');

const ZymbitKeyring = require('../src/eth-zymbit-keyring');
const { bytesToHex } = require('ethereum-cryptography/utils');
const { ethers } = require('ethers')
const zk = new zkJS.zkObj()
const EthereumTx = require('ethereumjs-tx').Transaction

const getStartingSlots = () => {
    const startingSlots = []
    const slotsBuffer = zk.getAllocSlotsList(false)
    for (let i = 0; i < slotsBuffer.length / 4; i++) {
      const slot = slotsBuffer.readInt32LE(4 * i)
      if (slot > 15) startingSlots.push(slot)
    }
    return startingSlots
}

const removeKeysCreated = startingSlots => {
    const slotsBuffer = zk.getAllocSlotsList(false)
    for (let i = 0; i < slotsBuffer.length / 4; i++) {
      const slot = slotsBuffer.readInt32LE(4 * i)
      if (slot > 15 && !startingSlots.includes(slot)) zk.removeKey(slot, false)
    }
}

describe('ZymbitKeyring', function () {
    let startingSlots, wallet_name, master_slot, opts, keyring

    before('Setup Keyring', function(done){
        this.timeout(10000)
        startingSlots = getStartingSlots()
        wallet_name = "TestWallet"
        const { slot, mnemonic } = zk.genWalletMasterSeedWithBIP39("secp256k1", wallet_name, "", "")
        master_slot = slot
        opts = {
            wallet_name,
            master_slot: slot
        }
        keyring = new ZymbitKeyring(opts)
        done()
    })

    describe('constructor', function() {
       
        let baseSlotDetails
        before('Get base_slot details', function(){
            baseSlotDetails = zk.getWalletNodeAddrFromKeySlot(keyring.base_slot)
        })
        it('constructs', function () {
            assert.equal(typeof keyring, 'object')
        })
        it('initialzes keyring correctly', function () {
            assert.equal(ZymbitKeyring.type, 'Zymbit Hardware Wallet')
            assert.equal(ZymbitKeyring.base_path, baseSlotDetails.node_address)
            assert.equal(keyring.wallet_name, wallet_name)
            assert.equal(keyring.master_slot, master_slot)
            assert.equal(keyring.wallet_name, baseSlotDetails.wallet_name)
            assert.deepEqual(keyring.account_slots, [])
        })
    })

    describe('addAccounts', function() {
        let numAccounts, accounts, publicKeys
        before('Add accounts', function(done){
            this.timeout(10000)
            numAccounts = 3
            accounts = keyring.addAccounts(numAccounts)
            publicKeys = keyring.account_slots.map(account_slot => zk.exportPubKey(account_slot.slot, false))
            done()
        })

        it('returns correct number of addresses', function () {
            assert.equal(accounts.length, numAccounts)
        })

        it('returns valid ethereum addresses', function () {
            accounts.forEach(account =>{
                assert.equal(ethers.isAddress(account), true)
            })
        })  

        it('returns correct addresses', function () {
            accounts.forEach((account, index) => {
                assert.equal(account, ethers.computeAddress('0x' + bytesToHex(publicKeys[index])))
            })
        })
    })

    describe('getAccounts', function() {
        let accounts, publicKeys
        before('Get accounts', function(done){
            this.timeout(10000)
            accounts = keyring.getAccounts()
            publicKeys = keyring.account_slots.map(account_slot => zk.exportPubKey(account_slot.slot, false))
            done()
        })
        
        it('returns correct number of accounts', function () {
            assert.equal(accounts.length, keyring.account_slots.length)
        })

        it('returns correct addresses', function () {
            accounts.forEach((account, index) => {
                assert.equal(account, ethers.computeAddress('0x' + bytesToHex(publicKeys[index])))
            })
        })
    })

    describe('signTransaction', function() {
        let accounts, signerIndex, signer, transaction
        before('Setup Transaction', function(done){
            this.timeout(10000)
            accounts = keyring.getAccounts()
            signerIndex = 0
            signer = accounts[signerIndex]
            const txParams = {
                from: fromAccount,
                nonce: '0x00',
                gasPrice: '0x09184e72a000',
                gasLimit: '0x2710',
                to: accounts[2],
                value: '0x1000',
              };
            
            transaction = new EthereumTx(txParams, { chain: 'mainnet'})
            done()
        })

        
    })

    describe('serialize', function() {
        let serializedKeyring
        before('Get accounts', function(){
            serializedKeyring = keyring.serialize()
        })
        it('returns correct wallet_name and slot', function () {
            assert.equal(serializedKeyring.wallet_name, wallet_name)
            assert.equal(serializedKeyring.master_slot, master_slot)
        })
    })

    describe('deserialize', function() {
        let temp_wallet_name, temp_slot, temp_mnemonic, temp_opts, temp_keyring
        before('Setup Temp Keyring', function(done){
            this.timeout(10000)
            temp_wallet_name = "TestWallet1"
            temp_slot, temp_mnemonic = zk.genWalletMasterSeedWithBIP39("secp256k1", temp_wallet_name, "", "")
            temp_opts = {
                wallet_name: temp_wallet_name,
                master_slot: temp_slot
            }
            temp_keyring = new ZymbitKeyring(temp_opts)
            done()
        })

        it('restores keyring\'s state', function (done) {
            this.timeout(10000)
            const serializedKeyring = keyring.serialize()
            temp_keyring.deserialize(serializedKeyring)
            assert.deepEqual(keyring, temp_keyring)
            done()
        })
    })

    after('Cleanup after testing', function(done){
        this.timeout(10000)
        removeKeysCreated(startingSlots)
        done()
    })

})

