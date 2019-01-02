# RandomHash Node.js Module

This module lets you hash a single PascalCoin block header. It was developed to be used by [PascalCoin Free Pool](https://github.com/PascalCoinFreePool/PascalCoinFreePool) in order to verify shares.

## Usage

The RandomHash algo can be called from Node.js in either sync, or async mode:

```
var randomhash = require("node-randomhash");

var blockHeader = Buffer.concat([
    Buffer.from(blockRpcData.part1, "hex"),
    Buffer.from(payload, "hex"),
    Buffer.from(blockRpcData.part3, "hex"),
    Buffer.from(nTime, "hex").swap32(),
    Buffer.from(nonce, "hex").swap32()
]);

// Sync
var blockHash = randomhash.hashSync(blockHeader);
var blockHashString = blockHash.toString("hex");

// Async
randomhash.hashAsync(blockHeader, function (err, blockHash) {
    if(err) {
        // An error occurred during hashing
    }

    var blockHashString = blockHash.toString("hex");
});

```

## Credits

The C++ code of this module was adapted from Polyminer's excellent RHMiner: https://github.com/polyminer1/rhminer

## Donations

Donations are accepted at 573198-21