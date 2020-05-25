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
#include "catapult/ionet/BatchPacketReader.h"
#include "catapult/ionet/PacketIo.h"
#include "catapult/utils/TimeSpan.h"
#include "tests/test/nodeps/Waits.h"
#include <queue>
#include <thread>
#include <vector>

namespace catapult { namespace mocks {

	/// Mock PacketIo that can be configured to pass specific values to read and write callbacks.
	class MockPacketIo : public ionet::PacketIo, public ionet::BatchPacketReader {
	public:
		/// Generates a read packet from the previously written packet or \c nullptr if no packets have been read.
		using GenerateReadPacket = std::function<std::shared_ptr<ionet::Packet> (const ionet::Packet*)>;

	public:
		/// Creates a new mock packet io.
		MockPacketIo()
				: m_numReads(0)
				, m_numWrites(0)
		{}

	public:
		void write(const ionet::PacketPayload& payload, const WriteCallback& callback) override {
			runDelayedAction([this, payload, callback]() {
				this->writeImpl(payload, callback);
			});
		}

		void read(const ReadCallback& callback) override {
			runDelayedAction([this, callback]() {
				this->readImpl(callback);
			});
		}

	public:
		void readMultiple(const ReadCallback& callback) override {
			runDelayedAction([this, callback]() {
				// call the callback with all queued reads
				while (!m_queuedReads.empty())
					this->readImpl(callback);
			});
		}

	private:
		void readImpl(const ReadCallback& callback) {
			++m_numReads;

			auto pair = m_queuedReads.front();
			m_queuedReads.pop();

			auto pPacket = invoke(pair.second);
			callback(pair.first, pPacket.get());
		}

		void writeImpl(const ionet::PacketPayload& payload, const WriteCallback& callback) {
			++m_numWrites;

			auto code = m_queuedWrites.front();
			m_queuedWrites.pop();

			m_writtenPackets.push_back(PayloadToPacket(payload));
			callback(code);
		}

		void runDelayedAction(const action& action) {
			// if there is no delay, execute the action immediately
			if (utils::TimeSpan() == m_ioDelay) {
				action();
				return;
			}

			std::thread([action, delay = m_ioDelay.millis()]() mutable {
				test::Sleep(static_cast<uint32_t>(delay));
				action();
			}).detach();
		}

	public:
		/// Gets the number of reads on this io.
		size_t numReads() const {
			return m_numReads;
		}

		/// Gets the number of writes on this io.
		size_t numWrites() const {
			return m_numWrites;
		}

		/// Gets a reference to the written packet at \a index.
		template<typename T>
		const T& writtenPacketAt(size_t index) const {
			return static_cast<const T&>(*m_writtenPackets[index]);
		}

	public:
		/// Sets the \a delay that io requests should wait before returning a response.
		/// \note The io must be kept alive externally until the delayed action is executed.
		void setDelay(const utils::TimeSpan& delay) {
			m_ioDelay = delay;
		}

		/// Queues a read operation to return \a code and a \c nullptr packet.
		void queueRead(ionet::SocketOperationCode code) {
			queueRead(code, GenerateReadPacket());
		}

		/// Queues a read operation to return \a code and a packet generated by \a generate.
		void queueRead(ionet::SocketOperationCode code, const GenerateReadPacket& generate) {
			m_queuedReads.emplace(code, generate);
		}

		/// Queues a write operation to return \a code.
		void queueWrite(ionet::SocketOperationCode code) {
			m_queuedWrites.push(code);
		}

	private:
		std::shared_ptr<ionet::Packet> invoke(const GenerateReadPacket& generate) const {
			if (!generate)
				return nullptr;

			return generate(m_writtenPackets.empty() ? nullptr : m_writtenPackets.back().get());
		}

	private:
		static std::shared_ptr<ionet::Packet> PayloadToPacket(const ionet::PacketPayload& payload) {
			const auto& header = payload.header();
			auto pPacket = ionet::CreateSharedPacket<ionet::Packet>(header.Size - sizeof(ionet::Packet));
			pPacket->Type = header.Type;

			size_t dataOffset = 0;
			for (const auto& buffer : payload.buffers()) {
				std::memcpy(pPacket->Data() + dataOffset, buffer.pData, buffer.Size);
				dataOffset += buffer.Size;
			}

			return pPacket;
		}

	private:
		size_t m_numReads;
		size_t m_numWrites;
		utils::TimeSpan m_ioDelay;
		std::queue<std::pair<ionet::SocketOperationCode, GenerateReadPacket>> m_queuedReads;
		std::queue<ionet::SocketOperationCode> m_queuedWrites;
		std::vector<std::shared_ptr<ionet::Packet>> m_writtenPackets;
	};
}}