# eth-zymbit-keyring

An implementation of MetaMask's [Keyring interface](https://github.com/MetaMask/eth-simple-keyring#the-keyring-class-protocol) for Zymbit hardware wallet devices ([HSM6](https://www.zymbit.com/hsm6/) and [SCM](https://www.zymbit.com/scm/)).

For the most part, it works the same way as [eth-hd-keyring](https://github.com/MetaMask/eth-hd-keyring), but uses a Zymbit HSM6 or SCM to store the keys and perform any signing operations. However, there are a some differences:

- It requires that you have an HSM6 or SCM attached to a supported embedded device (Raspberry Pi or Jetson Nano)
- All cryptographic operations involving keys are performed on the module itself
- Zymbit devices don't support exporting private keys, so it does not support the `exportAccount` method 

Installation
-------
Run the following command:

`yarn add eth-zymbit-keyring`

or

`npm install eth-zymbit-keyring`

Usage
-------

```
const ZymbitKeyring = require('eth-zymbit-keyring')
const options = {
    wallet_name: "MyExampleWallet"
    master_slot: 16
}
const keyring = new ZymbitKeyring(options);
```

### constructor( options )

The constructor receives an options object that requires either a valid `wallet_name` or `master_slot` property. In order to use this, you must have created or recovered a BIP39 wallet through [Zymbit's API](https://docs.zymbit.com/tutorials/digital-wallet/wallet-example/).

## Keyring Instance Methods

All below instance methods return Promises to allow asynchronous resolution.

### serialize()

In this method, you must return any JSON-serializable JavaScript object that you like. It will be encoded to a string, encrypted with the user's password, and stored to disk. This is the same object you will receive in the deserialize() method, so it should capture all the information you need to restore the Keyring's state.

### deserialize( object )

As discussed above, the deserialize() method will be passed the JavaScript object that you returned when the serialize() method was called.

### addAccounts( n = 1 )

The addAccounts(n) method is used to inform your keyring that the user wishes to create a new account. You should perform whatever internal steps are needed so that a call to serialize() will persist the new account, and then return an array of the new account addresses.

The method may be called with or without an argument, specifying the number of accounts to create. You should generally default to 1 per call.

### getAccounts()

When this method is called, you must return an array of hex-string addresses for the accounts that your Keyring is able to sign for.

### signTransaction(address, transaction)

This method will receive a hex-prefixed, all-lowercase address string for the account you should sign the incoming transaction with.

For your convenience, the transaction is an instance of ethereumjs-tx, (https://github.com/ethereumjs/ethereumjs-tx) so signing can be as simple as:

```
transaction.sign(privateKey)
```

You must return a valid signed ethereumjs-tx (https://github.com/ethereumjs/ethereumjs-tx) object when complete, it can be the same transaction you received.

### signMessage(address, data)

The `eth_sign` method will receive the incoming data, alread hashed, and must sign that hash, and then return the raw signed hash.

Testing
-------
Run the following command:

`npm test`