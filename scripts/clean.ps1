function removeSilent {
	param(
		$dirName
	)
	rm -r $dirName 2>$null
}

removeSilent('./deps/common/build/*')
removeSilent('./deps/skse64/build/*')
removeSilent('./include/common')
removeSilent('./lib')
removeSilent('./x64')