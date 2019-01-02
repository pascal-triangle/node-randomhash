{
    "targets": [
	{
	    "target_name": "randomhash",
	    "sources": [
		"lib/Pascal/RandomHash.cpp",
		"lib/utils.cpp",
		"lib/main.cpp",
	    ],
	    "include_dirs": [
		"lib",
		"lib/Pascal",
		"lib/Algo",
		"<!(node -e \"require('nan')\")"
	    ],
	    "cflags_cc": [
		"-std=c++11 -fexceptions"
	    ],
	}
    ]
}
