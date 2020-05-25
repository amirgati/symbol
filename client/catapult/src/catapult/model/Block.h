/**
*** Copyright (c) 2016-present,
*** Jaguar0625, gimre, BloodyRookie, Tech Bureau, Corp. All rights reserved.
***
*** This file is part of Catapult.
***
*** Catapult is free software: you can redistribute it and/or modify
*** it under the terms of the GNU Lesser General Public License as published by
*** the Free Software Foundation, either version 3 of the License, or
*** (at your option) any later version.
***
*** Catapult is distributed in the hope that it will be useful,
*** but WITHOUT ANY WARRANTY; without even the implied warranty of
*** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
*** GNU Lesser General Public License for more details.
***
*** You should have received a copy of the GNU Lesser General Public License
*** along with Catapult. If not, see <http://www.gnu.org/licenses/>.
**/

#pragma once
#include "Transaction.h"
#include "TransactionContainer.h"
#include "VerifiableEntity.h"
#include "catapult/crypto/Vrf.h"
#include "catapult/types.h"
#include <memory>
#include <vector>

namespace catapult { namespace model {

#pragma pack(push, 1)

	/// VRF proof for the verifiable random function.
	struct PackedVrfProof {
		/// Gamma.
		crypto::ProofGamma Gamma;

		/// Verification hash.
		crypto::ProofVerificationHash VerificationHash;

		/// Scalar.
		crypto::ProofScalar Scalar;
	};

	/// Binary layout for a block header.
	struct BlockHeader : public VerifiableEntity {
	public:
		/// Block format version.
		static constexpr int Current_Version = 1;

		/// Size of the footer that can be skipped when signing/verifying.
		static constexpr size_t Footer_Size = sizeof(uint32_t);

	public:
		/// Block height.
		catapult::Height Height;

		/// Number of milliseconds elapsed since creation of nemesis block.
		catapult::Timestamp Timestamp;

		/// Block difficulty.
		catapult::Difficulty Difficulty;

		/// Generation hash proof.
		PackedVrfProof GenerationHashProof;

		/// Previous block hash.
		Hash256 PreviousBlockHash;

		/// Hash of the transactions in this block.
		Hash256 TransactionsHash;

		/// Hash of the receipts generated by this block.
		Hash256 ReceiptsHash;

		/// Hash of the global chain state at this block.
		Hash256 StateHash;

		/// Beneficiary address designated by harvester.
		Address BeneficiaryAddress;

		/// Fee multiplier applied to block transactions.
		BlockFeeMultiplier FeeMultiplier;

		/// Reserved padding to align end of BlockHeader on 8-byte boundary.
		uint32_t BlockHeader_Reserved1;
	};

	/// Binary layout for a block.
	struct Block : public TransactionContainer<BlockHeader, Transaction> {};

#pragma pack(pop)

	/// Gets the number of bytes containing transaction data according to \a header.
	size_t GetTransactionPayloadSize(const BlockHeader& header);

	/// Checks the real size of \a block against its reported size and returns \c true if the sizes match.
	/// \a registry contains all known transaction types.
	bool IsSizeValid(const Block& block, const TransactionRegistry& registry);
}}