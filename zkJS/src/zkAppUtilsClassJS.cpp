#include "zkAppUtilsClassJS.h"

//---------Utility Functions--------------------------------------------------

ZK_EC_KEY_TYPE castStringToZkECKeyType(std::string key_type)
{
   std::transform(key_type.begin(), key_type.end(), key_type.begin(), [](unsigned char c){ return std::tolower(c); });
   if(key_type == "nistp256")
   {
      return ZK_NISTP256;
   }
   else if (key_type == "secp256k1")
   {
      return ZK_SECP256K1;
   }
   else if (key_type == "ed25519")
   {
      return ZK_ED25519;
   }
   else if (key_type == "x25519")
   {
      return ZK_X25519;
   }
   else
   {
      throw "Not a supported key type";
      //Napi::TypeError::New(env, "Not a supported key type").ThrowAsJavaScriptException();
   }
}

zkAppUtils::byteArray* castBufferToByteArray(Napi::Buffer<uint8_t> buffer)
{
   if (buffer.IsBuffer())
   {
      throw "Not a Buffer<uint8_t>";
   }

   return new zkAppUtils::byteArray(buffer.Data(), buffer.Data() + buffer.Length());
}

//---------zkObj--------------------------------------------------------------
//Zymkey Object class wrapped
Napi::FunctionReference zkObj::constructor;

Napi::Object zkObj::Init(Napi::Env env, Napi::Object exports)
{
  Napi::HandleScope scope(env);

  Napi::Function func = DefineClass(env, "zkObj", {
     InstanceMethod("getTime", &zkObj::getTime),
     InstanceMethod("exportPubKey", &zkObj::exportPubKey),
     InstanceMethod("storeForeignPubKey", &zkObj::exportPubKey),
     InstanceMethod("genKeyPair", &zkObj::genKeyPair),
     InstanceMethod("genEphemeralKeyPair", &zkObj::genEphemeralKeyPair),
     InstanceMethod("removeKey", &zkObj::removeKey),
     InstanceMethod("invalidateEphemeralKey", &zkObj::invalidateEphemeralKey),
     InstanceMethod("genECDSASigFromDigest", &zkObj::genECDSASigFromDigest),
     InstanceMethod("verifyECDSASigFromDigest", &zkObj::verifyECDSASigFromDigest),
     InstanceMethod("genWalletMasterSeedWithBIP39", &zkObj::genWalletMasterSeedWithBIP39),
     InstanceMethod("genWalletMasterSeedWithSLIP39", &zkObj::genWalletMasterSeedWithSLIP39),
     InstanceMethod("setGenSLIP39GroupInfo", &zkObj::setGenSLIP39GroupInfo),
     InstanceMethod("addGenSLIP39Member", &zkObj::addGenSLIP39Member),
     InstanceMethod("cancelSLIP39Session", &zkObj::cancelSLIP39Session),
     InstanceMethod("genOverSightWallet", &zkObj::genOverSightWallet),
     InstanceMethod("genWalletChildKey", &zkObj::genWalletChildKey),
     InstanceMethod("restoreWalletMasterSeedFromBIP39", &zkObj::restoreWalletMasterSeedFromBIP39),
     InstanceMethod("restoreWalletMasterSeedFromSLIP39", &zkObj::restoreWalletMasterSeedFromSLIP39),
     InstanceMethod("addRestoreSLIP39Mnemonic", &zkObj::addRestoreSLIP39Mnemonic),
     InstanceMethod("getWalletNodeAddrFromKeySlot", &zkObj::getWalletNodeAddrFromKeySlot),
     InstanceMethod("getWalletKeySlotFromNodeAddr", &zkObj::getWalletKeySlotFromNodeAddr),
     InstanceMethod("getAllocSlotsList", &zkObj::getAllocSlotsList),

  });

  constructor = Napi::Persistent(func);
  constructor.SuppressDestruct();

  exports.Set("zkObj", func);
  return exports;
}

zkObj::zkObj(const Napi::CallbackInfo& info) : Napi::ObjectWrap<zkObj>(info)
{
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);

  this->_zkCTX = new zkAppUtils::zkClass();
}

//wrapped member functions
Napi::Value zkObj::getTime(const Napi::CallbackInfo& info)
{
  Napi::Env env = info.Env();

  double time_stamp = this->_zkCTX->getTime();
  return Napi::Number::New(env, time_stamp);
}

Napi::Value zkObj::exportPubKey(const Napi::CallbackInfo& info)
{
   Napi::Env env = info.Env();

   std::stringstream exception;
   if (info.Length() != 2)
   {
      exception << "2 Arguments Expected. But " << info.Length() << " Arguments Were Provided.";
   }
   if (!info[0].IsNumber())
   {
      exception << " Int expected for Argument 1";
   }
   if (!info[1].IsBoolean())
   {
      exception << " Boolean expected for Argument 2";
   }

   if(!exception.str().empty())
   {
      Napi::TypeError::New(env, exception.str()).ThrowAsJavaScriptException();
   }

   Napi::Number pubkey_slot = info[0].As<Napi::Number>();
   Napi::Boolean slot_is_foreign = info[1].As<Napi::Boolean>();

   zkAppUtils::byteArray* pubkey_bytes = this->_zkCTX->exportPubKey(pubkey_slot, slot_is_foreign);
   return Napi::Buffer<uint8_t>::Copy(env, pubkey_bytes->data(), pubkey_bytes->size());
}

Napi::Value zkObj::storeForeignPubKey(const Napi::CallbackInfo& info)
{
   Napi::Env env = info.Env();

   std::stringstream exception;
   if (info.Length() != 2)
   {
      exception << "2 Arguments Expected. But " << info.Length() << " Arguments Were Provided.";
   }
   if (!info[0].IsString())
   {
      exception << " String expected for Argument 1";
   }
   if (!info[1].IsBuffer())
   {
      exception << " Buffer<uint8_t> expected for Argument 2";
   }

   if(!exception.str().empty())
   {
      Napi::TypeError::New(env, exception.str()).ThrowAsJavaScriptException();
   }

   ZK_EC_KEY_TYPE zk_key_type = castStringToZkECKeyType(info[0].As<Napi::String>());
   Napi::Buffer<uint8_t> input_data = info[1].As<Napi::Buffer<uint8_t>>();
   zkAppUtils::byteArray* pub_key = new zkAppUtils::byteArray(input_data.Data(), input_data.Data() + input_data.Length());

   int ret = this->_zkCTX->storeForeignPubKey(zk_key_type, *pub_key);
   return Napi::Number::New(env, ret);
}
Napi::Value zkObj::genKeyPair(const Napi::CallbackInfo& info)
{
   Napi::Env env = info.Env();

   std::stringstream exception;
   if (info.Length() != 1)
   {
      exception << "1 Argument Expected. But " << info.Length() << " Arguments Were Provided.";
   }
   if (!info[0].IsString())
   {
      exception << " String expected for Argument 1";
   }

   if(!exception.str().empty())
   {
      Napi::TypeError::New(env, exception.str()).ThrowAsJavaScriptException();
   }

   ZK_EC_KEY_TYPE zk_key_type = castStringToZkECKeyType(info[0].As<Napi::String>());

   int ret = this->_zkCTX->genKeyPair(zk_key_type);
   return Napi::Number::New(env, ret);
}
Napi::Value zkObj::genEphemeralKeyPair(const Napi::CallbackInfo& info)
{
   Napi::Env env = info.Env();

   std::stringstream exception;
   if (info.Length() != 1)
   {
      exception << "1 Argument Expected. But " << info.Length() << " Arguments Were Provided.";
   }
   if (!info[0].IsString())
   {
      exception << " String expected for Argument 1";
   }

   if(!exception.str().empty())
   {
      Napi::TypeError::New(env, exception.str()).ThrowAsJavaScriptException();
   }

   ZK_EC_KEY_TYPE zk_key_type = castStringToZkECKeyType(info[0].As<Napi::String>());

   this->_zkCTX->genEphemeralKeyPair(zk_key_type);
   return env.Null();
}
Napi::Value zkObj::removeKey(const Napi::CallbackInfo& info)
{
   Napi::Env env = info.Env();

   std::stringstream exception;
   if (info.Length() != 2)
   {
      exception << "2 Arguments Expected. But " << info.Length() << " Arguments Were Provided.";
   }
   if (!info[0].IsNumber())
   {
      exception << " Int expected for Argument 1";
   }
   if (!info[1].IsBoolean())
   {
      exception << " Boolean expected for Argument 2";
   }

   if(!exception.str().empty())
   {
      Napi::TypeError::New(env, exception.str()).ThrowAsJavaScriptException();
   }

   Napi::Number key_slot = info[0].As<Napi::Number>();
   Napi::Boolean slot_is_foreign = info[1].As<Napi::Boolean>();

   this->_zkCTX->removeKey(key_slot, slot_is_foreign);
   return env.Null();
}
Napi::Value zkObj::invalidateEphemeralKey(const Napi::CallbackInfo& info)
{
   Napi::Env env = info.Env();

   this->_zkCTX->invalidateEphemeralKey();
   return env.Null();
}

Napi::Value zkObj::genECDSASigFromDigest(const Napi::CallbackInfo& info)
{
   Napi::Env env = info.Env();
   Napi::Object napi_obj = Napi::Object::New(env);

   std::stringstream exception;
   if (info.Length() != 2 )
   {
      exception << "2 Arguments Expected. But " << info.Length() << " Arguments Were Provided.";
   }
   if (!info[0].IsBuffer())
   {
      exception << " Buffer<uint8_t> expected for Argument 1";
   }
   if (!info[1].IsNumber())
   {
      exception << " Number expected for Argument 2";
   }

   if(!exception.str().empty())
   {
      Napi::TypeError::New(env, exception.str()).ThrowAsJavaScriptException();
   }

   Napi::Buffer<uint8_t> digest_input = info[0].As<Napi::Buffer<uint8_t>>();
   zkAppUtils::byteArray* digest = new zkAppUtils::byteArray(digest_input.Data(), digest_input.Data() + digest_input.Length());

   Napi::Number slot = info[1].As<Napi::Number>();
   uint8_t rec_id;

   zkAppUtils::byteArray* signature = this->_zkCTX->genECDSASigFromDigest(*digest, rec_id, slot);
   Napi::Buffer<uint8_t> sig_buffer = Napi::Buffer<uint8_t>::Copy(env, signature->data(), signature->size());
   napi_obj.Set("signature", sig_buffer);
   napi_obj.Set("recovery_id", rec_id);
   return napi_obj;
}

Napi::Value zkObj::verifyECDSASigFromDigest(const Napi::CallbackInfo& info)
{
   Napi::Env env = info.Env();

   std::stringstream exception;
   if (info.Length() != 4)
   {
      exception << "4 Arguments Expected. But " << info.Length() << " Arguments Were Provided.";
   }
   if (!info[0].IsBuffer())
   {
      exception << " Buffer<uint8_t> expected for Argument 1";
   }
   if (!info[1].IsBuffer())
   {
      exception << " Buffer<uint8_t> expected for Argument 2";
   }
   if (!info[2].IsNumber())
   {
      exception << " Int expected for Argument 3";
   }
   if (!info[3].IsBoolean())
   {
      exception << " Boolean expected for Argument 4";
   }

   if(!exception.str().empty())
   {
      Napi::TypeError::New(env, exception.str()).ThrowAsJavaScriptException();
   }

   Napi::Buffer<uint8_t> digest_input = info[0].As<Napi::Buffer<uint8_t>>();
   zkAppUtils::byteArray* digest = new zkAppUtils::byteArray(digest_input.Data(), digest_input.Data() + digest_input.Length());

   Napi::Buffer<uint8_t> sig_input = info[1].As<Napi::Buffer<uint8_t>>();
   zkAppUtils::byteArray* signature = new zkAppUtils::byteArray(sig_input.Data(), sig_input.Data() + sig_input.Length());

   Napi::Number slot = info[2].As<Napi::Number>();
   Napi::Boolean is_foreign = info[2].As<Napi::Boolean>();

   bool ret = this->_zkCTX->verifyECDSASigFromDigest(*digest, *signature, slot, is_foreign);
   return Napi::Boolean::New(env, ret);
}

Napi::Value zkObj::genWalletMasterSeedWithBIP39(const Napi::CallbackInfo& info)
{
   Napi::Env env = info.Env();
   Napi::Object napi_obj = Napi::Object::New(env);

   std::stringstream exception;
   if (info.Length() < 2 || info.Length() > 5 )
   {
      exception << "2 Arguments Expected, 3 optional arguments. But " << info.Length() << " Arguments Were Provided.";
   }
   if (!info[0].IsString())
   {
      exception << " String expected for Argument 1";
   }
   if (!info[1].IsString())
   {
      exception << " String expected for Argument 2";
   }

   if(!exception.str().empty())
   {
      Napi::TypeError::New(env, exception.str()).ThrowAsJavaScriptException();
   }

   ZK_EC_KEY_TYPE zk_key_type = castStringToZkECKeyType(info[0].As<Napi::String>());;
   std::string wallet_name = std::string(info[1].As<Napi::String>());
   Napi::String rec_passphrase;
   Napi::String key_variant;
   zkAppUtils::byteArray* master_generator_key = new zkAppUtils::byteArray();
   std::string mnemonic_str;

   if(info.Length() > 2)
   {
      rec_passphrase = info[2].As<Napi::String>();
   }

   if(info.Length() > 3)
   {
      key_variant = info[3].As<Napi::String>();
   }

   if(info.Length() > 4)
   {
      Napi::Buffer<uint8_t> master_key_input = info[4].As<Napi::Buffer<uint8_t>>();
      master_generator_key = new zkAppUtils::byteArray(master_key_input.Data(), master_key_input.Data() + master_key_input.Length());
   }

   zkAppUtils::recoveryStrategyBIP39* rec_strat = new zkAppUtils::recoveryStrategyBIP39(rec_passphrase, key_variant);

   int ret = this->_zkCTX->genWalletMasterSeed(zk_key_type, wallet_name, *rec_strat, &mnemonic_str, *master_generator_key);
   napi_obj.Set("slot", ret);
   napi_obj.Set("mnemonic", mnemonic_str);
   return napi_obj;
}
Napi::Value zkObj::genWalletMasterSeedWithSLIP39(const Napi::CallbackInfo& info)
{
   Napi::Env env = info.Env();

   std::stringstream exception;
   if (info.Length() < 5 || info.Length() > 8 )
   {
      exception << "5 Arguments Expected, 3 optional arguments. But " << info.Length() << " Arguments Were Provided.";
   }
   if (!info[0].IsString())
   {
      exception << " String expected for Argument 1";
   }
   if (!info[1].IsString())
   {
      exception << " String expected for Argument 2";
   }
   if (!info[2].IsNumber())
   {
      exception << " Int expected for Argument 3";
   }
   if (!info[3].IsNumber())
   {
      exception << " Int expected for Argument 4";
   }
   if (!info[4].IsNumber())
   {
      exception << " Int expected for Argument 5";
   }

   if(!exception.str().empty())
   {
      Napi::TypeError::New(env, exception.str()).ThrowAsJavaScriptException();
   }

   ZK_EC_KEY_TYPE zk_key_type = castStringToZkECKeyType(info[0].As<Napi::String>());;
   std::string wallet_name = std::string(info[1].As<Napi::String>());
   Napi::Number group_count = info[2].As<Napi::Number>();
   Napi::Number group_threshold = info[3].As<Napi::Number>();
   Napi::Number iteration_exponent = info[4].As<Napi::Number>();
   Napi::String rec_passphrase;
   Napi::String key_variant;
   zkAppUtils::byteArray* master_generator_key = new zkAppUtils::byteArray();

   if(info.Length() > 5)
   {
      rec_passphrase = info[5].As<Napi::String>();
   }

   if(info.Length() > 6)
   {
      key_variant = info[6].As<Napi::String>();
   }

   if(info.Length() > 7)
   {
      Napi::Buffer<uint8_t> master_key_input = info[7].As<Napi::Buffer<uint8_t>>();
      master_generator_key = new zkAppUtils::byteArray(master_key_input.Data(), master_key_input.Data() + master_key_input.Length());
   }

   zkAppUtils::recoveryStrategySLIP39* rec_strat = new zkAppUtils::recoveryStrategySLIP39(group_count, group_threshold, iteration_exponent, rec_passphrase, key_variant);

   int ret = this->_zkCTX->genWalletMasterSeed(zk_key_type, wallet_name, *rec_strat, *master_generator_key);
   return Napi::Number::New(env, ret);
}
Napi::Value zkObj::setGenSLIP39GroupInfo(const Napi::CallbackInfo& info)
{
   Napi::Env env = info.Env();

   std::stringstream exception;
   if (info.Length() != 3)
   {
      exception << "3 Arguments Expected. But " << info.Length() << " Arguments Were Provided.";
   }
   if (!info[0].IsNumber())
   {
      exception << " Int expected for Argument 1";
   }
   if (!info[1].IsNumber())
   {
      exception << " Int expected for Argument 2";
   }
   if (!info[2].IsNumber())
   {
      exception << " Int expected for Argument 3";
   }

   if(!exception.str().empty())
   {
      Napi::TypeError::New(env, exception.str()).ThrowAsJavaScriptException();
   }

   Napi::Number group_index = info[0].As<Napi::Number>();
   Napi::Number member_count = info[1].As<Napi::Number>();
   Napi::Number member_threshold = info[2].As<Napi::Number>();

   int ret = this->_zkCTX->setGenSLIP39GroupInfo(group_index, member_count, member_threshold);
   return Napi::Number::New(env, ret);
}
Napi::Value zkObj::addGenSLIP39Member(const Napi::CallbackInfo& info)
{
   Napi::Env env = info.Env();
   Napi::Object napi_obj = Napi::Object::New(env);

   std::stringstream exception;
   if (info.Length() != 1)
   {
      exception << "1 Arguments Expected. But " << info.Length() << " Arguments Were Provided.";
   }
   if (!info[0].IsString())
   {
      exception << " String expected for Argument 1";
   }

   if(!exception.str().empty())
   {
      Napi::TypeError::New(env, exception.str()).ThrowAsJavaScriptException();
   }

   Napi::String passphrase = info[0].As<Napi::String>();
   std::string mnemonic_str;

   int ret = this->_zkCTX->addGenSLIP39Member(passphrase, &mnemonic_str);
   napi_obj.Set("return_code", ret);
   napi_obj.Set("mnemonic", mnemonic_str);
   return napi_obj;
}

Napi::Value zkObj::cancelSLIP39Session(const Napi::CallbackInfo& info)
{
   Napi::Env env = info.Env();

   int ret = this->_zkCTX->cancelSLIP39Session();
   return Napi::Number::New(env, ret);
}

Napi::Value zkObj::genOverSightWallet(const Napi::CallbackInfo& info)
{
   Napi::Env env = info.Env();

   std::stringstream exception;
   if (info.Length() != 6)
   {
      exception << "6 Arguments Expected. But " << info.Length() << " Arguments Were Provided.";
   }
   if (!info[0].IsString())
   {
      exception << " String expected for Argument 1";
   }
   if (!info[1].IsBuffer())
   {
      exception << " Buffer<uint8_t> expected for Argument 2";
   }
   if (!info[2].IsBuffer())
   {
      exception << " Buffer<uint8_t> expected for Argument 3";
   }
   if (!info[3].IsString())
   {
      exception << " String expected for Argument 4";
   }
   if (!info[4].IsBuffer())
   {
      exception << " String expected for Argument 5";
   }
   if (!info[5].IsBuffer())
   {
      exception << " String expected for Argument 6";
   }

   if(!exception.str().empty())
   {
      Napi::TypeError::New(env, exception.str()).ThrowAsJavaScriptException();
   }

   ZK_EC_KEY_TYPE zk_key_type = castStringToZkECKeyType(info[0].As<Napi::String>());
   Napi::Buffer<uint8_t> napi_pub_key = info[1].As<Napi::Buffer<uint8_t>>();
   zkAppUtils::byteArray* pub_key = new zkAppUtils::byteArray(napi_pub_key.Data(), napi_pub_key.Data() + napi_pub_key.Length());
   Napi::Buffer<uint8_t> napi_chain_code = info[2].As<Napi::Buffer<uint8_t>>();
   zkAppUtils::byteArray* chain_code = new zkAppUtils::byteArray(napi_chain_code.Data(), napi_chain_code.Data() + napi_chain_code.Length());
   std::string node_addr = std::string(info[3].As<Napi::String>());
   std::string wallet_name = std::string(info[4].As<Napi::String>());
   std::string key_variant = std::string(info[5].As<Napi::String>());

   int slot = this->_zkCTX->genOverSightWallet(zk_key_type, *pub_key, *chain_code, node_addr, wallet_name, key_variant);
   return Napi::Number::New(env, slot);
}

Napi::Value zkObj::genWalletChildKey(const Napi::CallbackInfo& info)
{
   Napi::Env env = info.Env();
   Napi::Object napi_obj = Napi::Object::New(env);

   std::stringstream exception;
   if (info.Length() != 4)
   {
      exception << "4 Arguments Expected. But " << info.Length() << " Arguments Were Provided.";
   }
   if (!info[0].IsNumber())
   {
      exception << " Int expected for Argument 1";
   }
   if (!info[1].IsNumber())
   {
      exception << " Int expected for Argument 2";
   }
   if (!info[2].IsBoolean())
   {
      exception << " Boolean expected for Argument 3";
   }
   if (!info[3].IsBoolean())
   {
      exception << " Boolean expected for Argument 4";
   }

   if(!exception.str().empty())
   {
      Napi::TypeError::New(env, exception.str()).ThrowAsJavaScriptException();
   }

   Napi::Number parent_slot = info[0].As<Napi::Number>();
   Napi::Number node_index = info[1].As<Napi::Number>();
   Napi::Boolean is_hardened = info[2].As<Napi::Boolean>();
   Napi::Boolean return_chain_code = info[3].As<Napi::Boolean>();
   zkAppUtils::byteArray chain_code;

   int child_slot = this->_zkCTX->genWalletChildKey(parent_slot, node_index, is_hardened, return_chain_code, &chain_code);
   Napi::Buffer<uint8_t> napi_chain_code = Napi::Buffer<uint8_t>::Copy(env, chain_code.data(), chain_code.size());
   napi_obj.Set("slot", child_slot);
   napi_obj.Set("chain_code", napi_chain_code);
   return napi_obj;
}
Napi::Value zkObj::restoreWalletMasterSeedFromBIP39(const Napi::CallbackInfo& info)
{
   Napi::Env env = info.Env();

   std::stringstream exception;
   if (info.Length() < 3 || info.Length() > 6 )
   {
      exception << "3 Arguments Expected, 3 optional arguments. But " << info.Length() << " Arguments Were Provided.";
   }
   if (!info[0].IsString())
   {
      exception << " String expected for Argument 1";
   }
   if (!info[1].IsString())
   {
      exception << " String expected for Argument 2";
   }
   if (!info[2].IsString())
   {
      exception << " String expected for Argument 3";
   }

   if(!exception.str().empty())
   {
      Napi::TypeError::New(env, exception.str()).ThrowAsJavaScriptException();
   }

   ZK_EC_KEY_TYPE zk_key_type = castStringToZkECKeyType(info[0].As<Napi::String>());;
   std::string wallet_name = std::string(info[1].As<Napi::String>());
   std::string mnemonic_str = std::string(info[2].As<Napi::String>());
   Napi::String rec_passphrase;
   Napi::String key_variant;
   zkAppUtils::byteArray* master_generator_key = new zkAppUtils::byteArray();

   if(info.Length() > 3)
   {
      rec_passphrase = info[3].As<Napi::String>();
   }

   if(info.Length() > 4)
   {
      key_variant = info[4].As<Napi::String>();
   }

   if(info.Length() > 5)
   {
      Napi::Buffer<uint8_t> master_key_input = info[5].As<Napi::Buffer<uint8_t>>();
      master_generator_key = new zkAppUtils::byteArray(master_key_input.Data(), master_key_input.Data() + master_key_input.Length());
   }

   zkAppUtils::recoveryStrategyBIP39* rec_strat = new zkAppUtils::recoveryStrategyBIP39(rec_passphrase, key_variant);

   int slot = this->_zkCTX->restoreWalletMasterSeedFromMnemonic(zk_key_type, wallet_name, *master_generator_key, *rec_strat, mnemonic_str);
   return Napi::Number::New(env, slot);
}
Napi::Value zkObj::restoreWalletMasterSeedFromSLIP39(const Napi::CallbackInfo& info)
{
   Napi::Env env = info.Env();

   std::stringstream exception;
   if (info.Length() < 5 || info.Length() > 8 )
   {
      exception << "5 Arguments Expected, 3 optional arguments. But " << info.Length() << " Arguments Were Provided.";
   }
   if (!info[0].IsString())
   {
      exception << " String expected for Argument 1";
   }
   if (!info[1].IsString())
   {
      exception << " String expected for Argument 2";
   }
   if (!info[2].IsNumber())
   {
      exception << " Int expected for Argument 3";
   }
   if (!info[3].IsNumber())
   {
      exception << " Int expected for Argument 4";
   }
   if (!info[4].IsNumber())
   {
      exception << " Int expected for Argument 5";
   }

   if(!exception.str().empty())
   {
      Napi::TypeError::New(env, exception.str()).ThrowAsJavaScriptException();
   }

   ZK_EC_KEY_TYPE zk_key_type = castStringToZkECKeyType(info[0].As<Napi::String>());;
   std::string wallet_name = std::string(info[1].As<Napi::String>());
   Napi::Number group_count = info[2].As<Napi::Number>();
   Napi::Number group_threshold = info[3].As<Napi::Number>();
   Napi::Number iteration_exponent = info[4].As<Napi::Number>();
   Napi::String rec_passphrase;
   Napi::String key_variant;
   zkAppUtils::byteArray* master_generator_key = new zkAppUtils::byteArray();

   if(info.Length() > 5)
   {
      rec_passphrase = info[5].As<Napi::String>();
   }

   if(info.Length() > 6)
   {
      key_variant = info[6].As<Napi::String>();
   }

   if(info.Length() > 7)
   {
      Napi::Buffer<uint8_t> master_key_input = info[7].As<Napi::Buffer<uint8_t>>();
      master_generator_key = new zkAppUtils::byteArray(master_key_input.Data(), master_key_input.Data() + master_key_input.Length());
   }

   zkAppUtils::recoveryStrategySLIP39* rec_strat = new zkAppUtils::recoveryStrategySLIP39(group_count, group_threshold, iteration_exponent, rec_passphrase, key_variant);

   int ret = this->_zkCTX->restoreWalletMasterSeedFromMnemonic(zk_key_type, wallet_name, *master_generator_key, *rec_strat);
   return Napi::Number::New(env, ret);
}
Napi::Value zkObj::addRestoreSLIP39Mnemonic(const Napi::CallbackInfo& info)
{
   Napi::Env env = info.Env();

   std::stringstream exception;
   if (info.Length() < 1 || info.Length() > 2)
   {
      exception << "1 Arguments Expected, 1 optional argument. But " << info.Length() << " Arguments Were Provided.";
   }
   if (!info[0].IsString())
   {
      exception << " String expected for Argument 1";
   }

   if(!exception.str().empty())
   {
      Napi::TypeError::New(env, exception.str()).ThrowAsJavaScriptException();
   }

   std::string mnemonic_str = std::string(info[0].As<Napi::String>());
   std::string passphrase;
   if(info.Length() > 1)
   {
      passphrase = std::string(info[1].As<Napi::String>());
   }

   int slot = this->_zkCTX->addRestoreSLIP39Mnemonic(passphrase, mnemonic_str);
   return Napi::Number::New(env, slot);
}
Napi::Value zkObj::getWalletNodeAddrFromKeySlot(const Napi::CallbackInfo& info)
{
   Napi::Env env = info.Env();
   Napi::Object napi_obj = Napi::Object::New(env);

   std::stringstream exception;
   if (info.Length() != 1)
   {
      exception << "1 Arguments Expected. But " << info.Length() << " Arguments Were Provided.";
   }
   if (!info[0].IsNumber())
   {
      exception << " Int expected for Argument 1";
   }

   if(!exception.str().empty())
   {
      Napi::TypeError::New(env, exception.str()).ThrowAsJavaScriptException();
   }

   Napi::Number slot = info[0].As<Napi::Number>();
   std::string node_addr;
   std::string wallet_name;

   this->_zkCTX->getWalletNodeAddrFromKeySlot(slot, &node_addr, &wallet_name);
   napi_obj.Set("node_address", node_addr);
   napi_obj.Set("wallet_name", wallet_name);
   return napi_obj;
}
Napi::Value zkObj::getWalletKeySlotFromNodeAddr(const Napi::CallbackInfo& info)
{
   Napi::Env env = info.Env();

   std::stringstream exception;
   if (info.Length() != 2)
   {
      exception << "1 Arguments Expected. But " << info.Length() << " Arguments Were Provided.";
   }
   if (!info[0].IsString())
   {
      exception << " String expected for Argument 1";
   }
   if (!info[1].IsNumber() && !info[1].IsString())
   {
      exception << " String(wallet_name) or Int(master_seed_slot) expected for Argument 2";
   }

   if(!exception.str().empty())
   {
      Napi::TypeError::New(env, exception.str()).ThrowAsJavaScriptException();
   }

   std::string node_addr = std::string(info[0].As<Napi::String>());
   std::string wallet_name;
   int master_seed_slot = 0;
   if(info[1].IsString())
   {
      wallet_name = std::string(info[1].As<Napi::String>());
   }
   else if (info[1].IsNumber())
   {
      master_seed_slot = info[1].As<Napi::Number>();
   }

   int slot = this->_zkCTX->getWalletKeySlotFromNodeAddr(node_addr, wallet_name, master_seed_slot);
   return Napi::Number::New(env, slot);
}
Napi::Value zkObj::getAllocSlotsList(const Napi::CallbackInfo& info)
{
   Napi::Env env = info.Env();

   std::stringstream exception;
   if (info.Length() != 1)
   {
      exception << "1 Arguments Expected. But " << info.Length() << " Arguments Were Provided.";
   }
   if (!info[0].IsBoolean())
   {
      exception << " Boolean expected for Argument 1";
   }

   if(!exception.str().empty())
   {
      Napi::TypeError::New(env, exception.str()).ThrowAsJavaScriptException();
   }

   Napi::Boolean is_foreign = info[0].As<Napi::Boolean>();

   zkAppUtils::intArray* slots_list = this->_zkCTX->getAllocSlotsList(is_foreign);
   return Napi::Buffer<int>::Copy(env, slots_list->data(), slots_list->size());
}

//Registering the node - module
Napi::Object zkOpen(Napi::Env env, Napi::Object exports)
{
  return zkObj::Init(env, exports);
}

NODE_API_MODULE(zkAppUtils, zkOpen);
