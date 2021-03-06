// Copyright (c) 2019 leosocy. All rights reserved.
// Use of this source code is governed by a MIT-style license
// that can be found in the LICENSE file.

#ifndef EDCC_SRC_CONFIG_READER_H_
#define EDCC_SRC_CONFIG_READER_H_

#include <map>
#include "config/config.h"
#include "edcc/status.h"

namespace edcc {

class ConfigReader {
 public:
  virtual ~ConfigReader() = default;;
  virtual Status LoadAndValidate() = 0;
  const EncoderConfig& GetEncoderConfig() { return encoder_cfg_; }

  // we'll validate when set config value.
  Status SetImageSize(uint8_t size);
  Status SetGaborKernelSize(uint8_t size);
  Status SetLaplaceKernelSize(uint8_t size);
  Status SetGaborDirections(uint8_t num);

 protected:
  EncoderConfig encoder_cfg_;
};

class SimpleConfigReader : public ConfigReader {
 public:
  explicit SimpleConfigReader(const EncoderConfig& config);
  Status LoadAndValidate() override;
};

// TODO: YamlConfigReader/JsonConfigReader

}  // namespace edcc

#endif  // EDCC_SRC_CONFIG_READER_H_