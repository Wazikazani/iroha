/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "interfaces/iroha_internal/transaction_batch.hpp"
#include "validators/field_validator.hpp"
#include "validators/transaction_validator.hpp"
#include "validators/transactions_collection/batch_order_validator.hpp"

namespace shared_model {
  namespace interface {

    /**
     * check if all transactions belong to the same batch
     * @param txs transactions to be checked
     * @return true if all transactions from the same batch and false otherwise
     */
    bool allTxsInSameBatch(const types::SharedTxsCollectionType &txs) {
      // if txs is empty then true
      if (txs.empty()) {
        return true;
      }

      // take batch meta of the first transaction and compare it with batch
      // metas of remaining transactions
      auto batch_meta = txs.front()->batchMeta();
      if (not batch_meta) {
        return false;
      }

      for (auto &it = ++txs.begin(); it < txs.end(); it++) {
        auto rhs_batch_meta = it->get()->batchMeta();
        if (not rhs_batch_meta or **batch_meta != **rhs_batch_meta) {
          return false;
        }
      }
      return true;
    };

    template <typename TransactionValidator, typename OrderValidator>
    iroha::expected::Result<TransactionBatch, std::string>
    TransactionBatch::createTransactionBatch(
        const types::SharedTxsCollectionType &transactions,
        const validation::TransactionsCollectionValidator<TransactionValidator,
                                                          OrderValidator>
            &validator) {
      auto answer = validator.validatePointers(transactions);
      if (not allTxsInSameBatch(transactions)) {
        answer.addReason(std::make_pair(
            "Transaction batch: ",
            std::vector<std::string>{
                "Provided transactions are not from the same batch"}));
      }

      if (answer.hasErrors()) {
        return iroha::expected::makeError(answer.reason());
      }
      return iroha::expected::makeValue(TransactionBatch(transactions));
    }

    template iroha::expected::Result<TransactionBatch, std::string>
    TransactionBatch::createTransactionBatch(
        const types::SharedTxsCollectionType &transactions,
        const validation::TransactionsCollectionValidator<
            validation::TransactionValidator<
                validation::FieldValidator,
                validation::CommandValidatorVisitor<
                    validation::FieldValidator>>,
            validation::BatchOrderValidator> &validator);

    template iroha::expected::Result<TransactionBatch, std::string>
    TransactionBatch::createTransactionBatch(
        const types::SharedTxsCollectionType &transactions,
        const validation::TransactionsCollectionValidator<
            validation::TransactionValidator<
                validation::FieldValidator,
                validation::CommandValidatorVisitor<
                    validation::FieldValidator>>,
            validation::AnyOrderValidator> &validator);

    template <typename TransactionValidator>
    iroha::expected::Result<TransactionBatch, std::string>
    TransactionBatch::createTransactionBatch(
        std::shared_ptr<Transaction> transaction,
        const TransactionValidator &transaction_validator) {
      auto answer = transaction_validator.validate(*transaction);
      if (answer.hasErrors()) {
        return iroha::expected::makeError(answer.reason());
      }
      return iroha::expected::makeValue(
          TransactionBatch(types::SharedTxsCollectionType{transaction}));
    };

    template iroha::expected::Result<TransactionBatch, std::string>
    TransactionBatch::createTransactionBatch(
        std::shared_ptr<Transaction> transaction,
        const validation::TransactionValidator<
            validation::FieldValidator,
            validation::CommandValidatorVisitor<validation::FieldValidator>>
            &validator);

    const types::SharedTxsCollectionType& TransactionBatch::transactions() const {
      return transactions_;
    }

    const types::HashType& TransactionBatch::reducedHash() const {
      if (not reduced_hash_) {
        reduced_hash_ = TransactionBatch::calculateReducedBatchHash(
            transactions_ | boost::adaptors::transformed([](const auto &tx) {
              return tx->reducedHash();
            }));
      }
      return reduced_hash_.value();
    }

    types::HashType TransactionBatch::calculateReducedBatchHash(
        const boost::any_range<types::HashType, boost::forward_traversal_tag>
            &reduced_hashes) {
      std::stringstream concatenated_hash;
      for (const auto &hash : reduced_hashes) {
        concatenated_hash << hash.hex();
      }
      return types::HashType::fromHexString(concatenated_hash.str());
    }

  }  // namespace interface
}  // namespace shared_model