# eth-zymbit-keyring

An implementation of MetaMask's [Keyring interface](https://github.com/MetaMask/eth-simple-keyring#the-keyring-class-protocol) for Zymbit devices with built-in hardware wallets ([HSM6](https://www.zymbit.com/hsm6/) and [SCM](https://www.zymbit.com/scm/)).

## Table of Contents

- [Overview](#overview)
- [Installation](#installation)
- [Usage](#usage)
  - [ZymbitKeyring.type](#zymbitkeyringtype)
  - [constructor(options)](#constructoroptions)
- [Zymbit Keyring Instance Methods](#zymbit-keyring-instance-methods)
  - [serialize()](#serialize)
  - [deserialize(object)](#deserializeobject)
  - [addAccounts(n = 1)](#addaccounts-n--1-)
  - [getAccounts()](#getaccounts)
  - [signTransaction(address, transaction)](#signtransactionaddress-transaction)
  - [signMessage(address, data)](#signmessageaddress-data)
  - [removeAccount(address)](#removeaccountaddress)
- [Testing](#testing)

## Overview

For the most part, it works the same way as [eth-hd-keyring](https://github.com/MetaMask/eth-hd-keyring), but uses a Zymbit HSM6 or SCM to store the keys and perform any signing operations. However, there are some differences:

- It requires that you have an HSM6 or SCM attached to a supported embedded device (Raspberry Pi or Jetson Nano)
- All cryptographic operations involving keys are performed on the module itself
- Zymbit devices don't support exporting private keys, so it does not support the `exportAccount()` method 

## Installation

Run the following command:

`yarn add eth-zymbit-keyring`

or

`npm install eth-zymbit-keyring`

## Usage

```
const ZymbitKeyring = require('eth-zymbit-keyring')
const options = {
    wallet_name: "MyExampleWallet",
    master_slot: 16
}
const keyring = new ZymbitKeyring(options);
```

### ZymbitKeyring.type

A class property that returns `Zymbit Hardware Wallet`

### constructor(options)

The constructor receives an options object that requires either a valid `wallet_name` or `master_slot` property. In order to use this, you must have created or recovered a BIP39 wallet through [Zymbit's API](https://docs.zymbit.com/tutorials/digital-wallet/wallet-example/).

## Zymbit Keyring Instance Methods

All below instance methods return Promises to allow asynchronous resolution.

### serialize()

Returns a JSON-serializable object with the `wallet_name` or `master_slot` properties of the keyring. This object can be passed into `deserialize()` to recover the state of the keyring. 

### deserialize(object)

As discussed above, the `deserialize()` method will be passed the JSON object that was returned when the `serialize()` method was called.

### addAccounts( n = 1 )

Used to add new accounts to the keyring starting from `m/44'/60'/0'/0` and incrementing the last index each time. Takes the number of accounts you want to add to the keyring `n` as a parameter (defaults to `n = 1`), and returns an array of size `n` including the Ethereum addresses of the accounts added.

### getAccounts()

Returns an array of hex-string addresses for the Ethereum accounts that the keyring is able to sign for.

### signTransaction(address, transaction)

Takes a hex-prefixed `address` string for the account you want to use to sign the incoming `transaction` with. The `transaction` is an instance of [ethereumjs-tx](https://github.com/ethereumjs/ethereumjs-tx). Returns a signed [ethereumjs-tx](https://github.com/ethereumjs/ethereumjs-tx) object when complete.

### signMessage(address, data)

Takes a pre-hashed message (`data`) and signs it using the account of the hex-prefixed `address` passed in. Returns the complete hex-prefixed ECDSA signature.

### removeAccount(address)

Removes the account corresponding to the hex-prefixed `address` passed in from the keyring and the Zymbit device.

## Testing

Run the following command:

`npm test`