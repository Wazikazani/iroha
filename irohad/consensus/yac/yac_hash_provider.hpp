/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_YAC_HASH_PROVIDER_HPP
#define IROHA_YAC_HASH_PROVIDER_HPP

#include <memory>
#include <string>

#include "interfaces/common_objects/types.hpp"
#include "ordering/on_demand_os_transport.hpp"

namespace shared_model {
  namespace interface {
    class Signature;
    class Block;
    class Proposal;
  }  // namespace interface
}  // namespace shared_model

namespace iroha {
  namespace consensus {
    namespace yac {

      class YacHash {
       public:
        YacHash(std::string proposal, std::string block)
            : proposal_hash(std::move(proposal)),
              block_hash(std::move(block)) {}

        YacHash() = default;

        /**
         * Hash computed from proposal
         */
        std::string proposal_hash;

        /**
         * Hash computed from block;
         */
        std::string block_hash;

        /// Consensus round
        ordering::transport::Round round;

        /**
         * Peer signature of block
         */
        std::shared_ptr<shared_model::interface::Signature> block_signature;

        bool operator==(const YacHash &obj) const {
          return proposal_hash == obj.proposal_hash
              and block_hash == obj.block_hash;
        };

        bool operator!=(const YacHash &obj) const {
          return not(*this == obj);
        };
      };

      /**
       * Provide methods related to hash operations in ya consensus
       */
      class YacHashProvider {
       public:
        /**
         * Make hash from block
         * @param block - for hashing
         * @return hashed value of block
         */
        virtual YacHash makeHash(
            const shared_model::interface::Block &block,
            const shared_model::interface::Proposal &proposal) const = 0;

        /**
         * Convert YacHash to model hash
         * @param hash - for converting
         * @return HashType of model hash
         */
        virtual shared_model::interface::types::HashType toModelHash(
            const YacHash &hash) const = 0;

        virtual ~YacHashProvider() = default;
      };
    }  // namespace yac
  }    // namespace consensus
}  // namespace iroha

#endif  // IROHA_YAC_HASH_PROVIDER_HPP
