# eth-zymbit-keyring

An implementation of MetaMask's [Keyring interface](https://github.com/MetaMask/eth-simple-keyring#the-keyring-class-protocol) for Zymbit hardware wallet devices ([HSM6](https://www.zymbit.com/hsm6/) and [SCM](https://www.zymbit.com/scm/)).

For the most part, it works the same way as [eth-hd-keyring](https://github.com/MetaMask/eth-hd-keyring), but uses a Zymbit HSM6 or SCM to store the keys and perform any signing operations. However, there are a some differences:

- It requires that you have an HSM6 or SCM attached and configured to a supported embedded device (Raspberry Pi or Jetson Nano)
- Zymbit devices don't support exporting private keys, so it does not support the `exportAccount` method 

Testing
-------
Run the following command:

```bash
npm test
```