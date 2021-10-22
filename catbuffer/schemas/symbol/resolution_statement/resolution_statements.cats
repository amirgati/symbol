import "receipts.cats"
import "resolution_statement/resolution_statement_types.cats"

# A Mosaic resolution statement links a namespace alias used in a transaction to the real mosaic id
# **at the time of the transaction**.
struct MosaicResolutionStatement
	inline Receipt

	# Unresolved mosaic.
	unresolved = UnresolvedMosaicId

	# Resolution entries.
	resolution_entries = array(MosaicResolutionEntry, __FILL__)

# An Address resolution statement links a namespace alias used in a transaction to the real address
# **at the time of the transaction**.
struct AddressResolutionStatement
	inline Receipt

	# Unresolved address.
	unresolved = UnresolvedAddress

	# Resolution entries.
	resolution_entries = array(AddressResolutionEntry, __FILL__)
