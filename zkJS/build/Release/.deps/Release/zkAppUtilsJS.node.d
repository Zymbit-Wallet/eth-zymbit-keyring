cmd_Release/zkAppUtilsJS.node := ln -f "Release/obj.target/zkAppUtilsJS.node" "Release/zkAppUtilsJS.node" 2>/dev/null || (rm -rf "Release/zkAppUtilsJS.node" && cp -af "Release/obj.target/zkAppUtilsJS.node" "Release/zkAppUtilsJS.node")