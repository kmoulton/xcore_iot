@Library('xmos_jenkins_shared_library@v0.14.2') _
getApproval()
pipeline {
    agent {
        dockerfile {
            args ""
        }
    }
    parameters { // Available to modify on the job page within Jenkins if starting a build
        string( // use to try different tools versions
            name: 'TOOLS_VERSION',
            defaultValue: '15.0.4',
            description: 'The tools version to build with (check /projects/tools/ReleasesTools/)'
        )
        booleanParam( // use to check results of rolling all conda deps forward
            name: 'UPDATE_ALL',
            defaultValue: false,
            description: 'Update all conda packages before building'
        )
    }
    options { // plenty of things could go here
        //buildDiscarder(logRotator(numToKeepStr: '10'))
        timestamps()
    }
    environment {
        XMOS_AIOT_SDK_PATH = "${env.WORKSPACE}"
    }
    stages {
        stage("Setup") {
            // Clone and install build dependencies
            steps {
                // clean auto default checkout
                sh "rm -rf *"
                // clone
                checkout([
                    $class: 'GitSCM',
                    branches: scm.branches,
                    doGenerateSubmoduleConfigurations: false,
                    extensions: [[$class: 'SubmoduleOption',
                                threads: 8,
                                timeout: 20,
                                shallow: false,
                                parentCredentials: true,
                                recursiveSubmodules: true],
                                [$class: 'CleanCheckout']],
                    userRemoteConfigs: [[credentialsId: 'xmos-bot',
                                        url: 'git@github.com:xmos/aiot_sdk']]
                ])
                // create venv
                sh "conda env create -q -p aiot_sdk_venv -f environment.yml"
                // Install xmos tools version
                sh "/XMOS/get_tools.py " + params.TOOLS_VERSION
            }
        }
        stage("Patch toolchain") {
            steps {
                sh "patch -N /XMOS/tools/15.0.4/XMOS/xTIMEcomposer/15.0.4/target/include/xcore/thread.h -i local_thread_mode_get_bits.patch"
            }
        }
        stage("Update environment") {
            // Roll all conda packages forward beyond their pinned versions
            when { expression { return params.UPDATE_ALL } }
            steps {
                sh "conda update --all -y -q -p aiot_sdk_venv"
            }
        }
        stage("Build documentation") {
            steps {
                dir('documents') {
                    sh '. activate ./aiot_sdk_venv && make clean linkcheck html SPHINXOPTS="-W --keep-going"'
                    dir('_build') {
                        archiveArtifacts artifacts: 'html/**/*', fingerprint: false
                        sh 'tar -czf docs_sdk.tgz html'
                        archiveArtifacts artifacts: 'docs_sdk.tgz', fingerprint: true
                    }
                }
            }
        }
    }
    post {
        cleanup {
            cleanWs()
        }
    }
}
