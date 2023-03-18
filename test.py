import zymkey

print(zymkey.client.get_slot_alloc_list())

# Create a master seed and return the BIP39 mnemonic
master_key_generator = bytearray("3xampleM@sterK3Y", 'utf-8')
wallet_name = "MyExampleWallet1"

child_slot = zymkey.client.gen_wallet_child_key(parent_key_slot = 22, index = 44, hardened = True)
print(child_slot)

# # Use the BIP39 recovery strategy to tell zymkey to return a mnemonic. Takes a base 64 encoded string for a BIP39 passphrase. Can be empty string.
# use_BIP39_recovery = zymkey.RecoveryStrategyBIP39()
# master_slot, BIP39_mnemonic = zymkey.client.gen_wallet_master_seed("secp256k1", master_key_generator, wallet_name, use_BIP39_recovery)
# print("Master Slot:%s\nBIP39 mnemonic (write this down!):\n%s" % (master_slot, BIP39_mnemonic))

# print(zymkey.client.get_wallet_key_slot("m/44'/60'/0'/0/0", 'MyExampleWallet'))

# for slot in zymkey.client.get_slot_alloc_list()[0]:
#     if(slot>15):
#         print(zymkey.client.get_wallet_node_addr(slot))
