#!/bin/bash
############################################################################################
## Copyright 2003, 2015 IBM Corp                                                          ##
##                                                                                        ##
## Redistribution and use in source and binary forms, with or without modification,       ##
## are permitted provided that the following conditions are met:                          ##
##      1.Redistributions of source code must retain the above copyright notice,          ##
##        this list of conditions and the following disclaimer.                           ##
##      2.Redistributions in binary form must reproduce the above copyright notice, this  ##
##        list of conditions and the following disclaimer in the documentation and/or     ##
##        other materials provided with the distribution.                                 ##
##                                                                                        ##
## THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS AND ANY EXPRESS       ##
## OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF        ##
## MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL ##
## THE AUTHOR OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,    ##
## EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF     ##
## SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) ##
## HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,  ##
## OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS  ##
## SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                           ##
############################################################################################
## File :        python-dmidecode.sh                                                      ##
## Description: This testcase tests python-dmidecode package    		          ##
## Author: Kingsuk Deb, kingsdeb@linux.vnet.ibm.com		                          ##
############################################################################################


#cd $(dirname $0)
#LTPBIN=${PWD%%/testcases/*}/testcases/bin
source $LTPBIN/tc_utils.source
DMIDECODE_TESTS_DIR="${LTPBIN%/shared}/python_dmidecode/unit-tests"

function dmidecodeConfig()
{
        tc_get_os_arch
        if [[ "$TC_OS_ARCH" == ppc* || "$TC_OS_ARCH" == s390x ]]; then
                tc_info "dmidecode is not supported this architecture"
                 exit 0
        fi

}

function tc_local_setup()
{
	tc_executes python rpm dmidecode
	tc_break_if_bad $? "python dmidecode or rpm is not installed" || return	
	sed -i '\_../src/pymap.xml_s_../src_/usr/share/python-dmidecode_' \
	$DMIDECODE_TESTS_DIR/unit 
}

function install_check()
{
      tc_check_package "python-dmidecode"
	tc_break_if_bad $? "python-dmidecode not installed"
}

function run_test()
{
	tc_register "python-dmidecode tests"
	pushd $DMIDECODE_TESTS_DIR &>/dev/null 
	python unit -vv >$stdout 2>$stderr
	tc_pass_or_fail $? "python-dmidecode tests failed"
	popd &>/dev/null
}

#
# main
#
dmidecodeConfig
tc_setup
TST_TOTAL=1
install_check && run_test 
