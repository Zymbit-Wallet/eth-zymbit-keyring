#include <napi.h>
#include "./zkAppUtilsClass.h"
#include <string>
#include <sstream>
#include <iostream>
#include <vector>

class zkObj : public Napi::ObjectWrap<zkObj>
{
 public:
  static Napi::Object Init(Napi::Env env, Napi::Object exports); //Init function for setting the export key to JS
  zkObj(const Napi::CallbackInfo& info); //Constructor to initialise

 private:
  static Napi::FunctionReference constructor; //reference to store the class definition that needs to be exported to JS
  Napi::Value getTime(const Napi::CallbackInfo& info);
  Napi::Value exportPubKey(const Napi::CallbackInfo& info);
  Napi::Value storeForeignPubKey(const Napi::CallbackInfo& info);
  Napi::Value genKeyPair(const Napi::CallbackInfo& info);
  Napi::Value genEphemeralKeyPair(const Napi::CallbackInfo& info);
  Napi::Value removeKey(const Napi::CallbackInfo& info);
  Napi::Value invalidateEphemeralKey(const Napi::CallbackInfo& info);
  Napi::Value genECDSASigFromDigest(const Napi::CallbackInfo& info);
  Napi::Value verifyECDSASigFromDigest(const Napi::CallbackInfo& info);
  Napi::Value genWalletMasterSeedWithBIP39(const Napi::CallbackInfo& info);
  Napi::Value genWalletMasterSeedWithSLIP39(const Napi::CallbackInfo& info);
  Napi::Value setGenSLIP39GroupInfo(const Napi::CallbackInfo& info);
  Napi::Value addGenSLIP39Member(const Napi::CallbackInfo& info);
  Napi::Value cancelSLIP39Session(const Napi::CallbackInfo& info);
  Napi::Value genOverSightWallet(const Napi::CallbackInfo& info);
  Napi::Value genWalletChildKey(const Napi::CallbackInfo& info);
  Napi::Value restoreWalletMasterSeedFromBIP39(const Napi::CallbackInfo& info);
  Napi::Value restoreWalletMasterSeedFromSLIP39(const Napi::CallbackInfo& info);
  Napi::Value addRestoreSLIP39Mnemonic(const Napi::CallbackInfo& info);
  Napi::Value getWalletNodeAddrFromKeySlot(const Napi::CallbackInfo& info);
  Napi::Value getWalletKeySlotFromNodeAddr(const Napi::CallbackInfo& info);
  Napi::Value getAllocSlotsList(const Napi::CallbackInfo& info);
  zkAppUtils::zkClass *_zkCTX; //internal instance of zk obj used to perform actual operations.
};
