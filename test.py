import zymkey 

packet =(bytes. ("b2c0a7451cf0bd1bdc86ee3fbbbfe6baf502670bae2a71cd093b37f6ff4c4102"))
print(packet)
   
signature, y_parity = zymkey.client.sign(
    packet, slot=22, return_recid=True)
N = 115792089237316195423570985008687907852837564279074904382605163141518161494337
r = int.from_bytes(signature[:32], "big")
s = int.from_bytes(signature[-32:], "big")

y_parity = bool(y_parity.value)
if((s*2) >= N):
    y_parity = not y_parity
    s = N - s

v = 1 * 2 + 35 + int(y_parity)

signature = '0x'+hex(r)[2:].zfill(64)+hex(s)[2:].zfill(64)+hex(v)[2:]

print(signature)