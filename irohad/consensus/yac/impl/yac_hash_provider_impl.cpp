/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "consensus/yac/impl/yac_hash_provider_impl.hpp"
#include "interfaces/iroha_internal/block.hpp"
#include "interfaces/iroha_internal/proposal.hpp"

namespace iroha {
  namespace consensus {
    namespace yac {

      YacHash YacHashProviderImpl::makeHash(
          const shared_model::interface::Block &block,
          const shared_model::interface::Proposal &proposal) const {
        YacHash result;
        result.proposal_hash = proposal.hash().hex();
        result.block_hash = block.hash().hex();
        result.block_signature = clone(block.signatures().front());
        return result;
      }

      shared_model::interface::types::HashType YacHashProviderImpl::toModelHash(
          const YacHash &hash) const {
        auto blob = shared_model::crypto::Blob::fromHexString(hash.block_hash);
        auto string_blob = shared_model::crypto::toBinaryString(blob);
        return shared_model::interface::types::HashType(string_blob);
      }
    }  // namespace yac
  }    // namespace consensus
}  // namespace iroha
