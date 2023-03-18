const zkJS = require('../zkJS/build/Release/zkAppUtilsJS.node');

const BIP44_ETH_BASE_PATH = `m/44'/60'/0'/0`
const type = 'Zymbit Hardware Wallet'
const zk = new zkJS.zkObj()



class ZymbitKeyring {

  constructor(opts = {}) {
    this.type = type;
    this.base_path = BIP44_ETH_BASE_PATH
    this.deserialize(opts);
  }

  serialize() {

  }

  deserialize(opts = {}) {
    if (!opts.wallet_name && (typeof (opts.master_slot) != 'number' || opts.master_slot < 16 || opts.master_slot > 512)) {
      throw new Error("Valid wallet name or master slot required")
    }
    if (opts.wallet_name) {
      try {
        this.master_slot = zk.getWalletKeySlotFromNodeAddr("m", opts.wallet_name)
        this.wallet_name = opts.wallet_name
      } catch {
        throw new Error("Invalid Wallet Name")
      }
    } else {
      try {
        const slotDetails = zk.getWalletNodeAddrFromKeySlot(opts.master_slot)
        if (slotDetails.node_address == 'm') {
          this.master_slot = opts.master_slot
          this.wallet_name = slotDetails.wallet_name
        } else {
          throw new Error
        }
      } catch (e) {
        throw new Error("Invalid Master Key Slot")
      }
    }
    this.base_slot = 0
    this.account_slots = []
    const slots = [...zk.getAllocSlotsList(false)].filter(slot => slot > 15)
    for (const slot of slots) {
      const slotDetails = zk.getWalletNodeAddrFromKeySlot(slot)
      if (slotDetails.wallet_name == this.wallet_name) {
        if (slotDetails.node_address == this.base_path) {
          this.base_slot = slot
        } else if (slotDetails.node_address.includes(this.base_path + "/")) {
          slotDetails.slot = slot
          delete slotDetails.wallet_name
          this.account_slots.push(slotDetails)
        }
      }
    }
    if (this.base_slot == 0) {
      let deepestPath = { node_address: "m", slot: this.master_slot }
      for (const slot of slots) {
        const slotDetails = zk.getWalletNodeAddrFromKeySlot(slot)
        if (slotDetails.wallet_name == this.wallet_name) {
          if (this.base_path.includes(slotDetails.node_address) && slotDetails.node_address.length > deepestPath.node_address.length) {
            slotDetails.slot = slot
            delete slotDetails.wallet_name
            deepestPath = slotDetails
          }
        }
      }
      this.base_slot = this.generateBasePathKey(deepestPath)
    }
    console.log(this)
    return Promise.resolve()
  }

  generateBasePathKey(deepestPath) {
    let slot = 0
    switch (deepestPath.node_address) {
      case `m`:
        slot = zk.genWalletChildKey(deepestPath.slot, 44, true, false)
        break;
      case `m/44'`:
        slot = zk.genWalletChildKey(deepestPath.slot, 60, true, false)
        break;
      case `m/44'/60'`:
        slot = zk.genWalletChildKey(deepestPath.slot, 0, true, false)
        break;
      case `m/44'/60'/0'`:
        slot = zk.genWalletChildKey(deepestPath.slot, 0, false, false)
        break;
      case `m/44'/60'/0'/0`:
        return deepestPath.slot
    }
    const slotDetails = zk.getWalletNodeAddrFromKeySlot(slot.slot)
    slotDetails.slot = slot.slot
    return this.generateBasePathKey(slotDetails)
  }

}

ZymbitKeyring.type = type;
module.exports = ZymbitKeyring;