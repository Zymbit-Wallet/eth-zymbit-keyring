const keyring = require('./eth-zymbit-keyring')

opts = {
    wallet_name: "MyExampleWallet",
    master_slot: 16
}

const zk = new keyring(opts)