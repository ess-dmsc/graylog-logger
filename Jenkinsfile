@Library('ecdc-pipeline')
import ecdcpipeline.ContainerBuildNode
import ecdcpipeline.PipelineBuilder

project = "graylog-logger"

// Set number of old builds to keep.
properties([[
  $class: 'BuildDiscarderProperty',
  strategy: [
    $class: 'LogRotator',
    artifactDaysToKeepStr: '',
    artifactNumToKeepStr: '10',
    daysToKeepStr: '',
    numToKeepStr: ''
  ]
]]);

clangformat_os = "debian11"
test_os = "ubuntu2204"

container_build_nodes = [
  'centos7': ContainerBuildNode.getDefaultContainerBuildNode('centos7-gcc11'),
  'debian11': ContainerBuildNode.getDefaultContainerBuildNode('debian11'),
  'ubuntu2204': ContainerBuildNode.getDefaultContainerBuildNode('ubuntu2204')
]

pipeline_builder = new PipelineBuilder(this, container_build_nodes)
pipeline_builder.activateEmailFailureNotifications()

builders = pipeline_builder.createBuilders { container ->
  pipeline_builder.stage("${container.key}: checkout") {
    dir(pipeline_builder.project) {
      checkout scm
    }
    // Copy source code to container
    container.copyTo(pipeline_builder.project, pipeline_builder.project)
  }  // stage

  pipeline_builder.stage("${container.key}: get dependencies") {
    container.sh """
      mkdir build
      cd build
      conan install --build outdated ../${pipeline_builder.project}
    """
  }  // stage

  pipeline_builder.stage("${container.key}: configure") {
    container.sh """
      cd build
      . ./activate_run.sh
      cmake ../${pipeline_builder.project} -DBUILD_EVERYTHING=ON
    """
  }  // stage

  pipeline_builder.stage("${container.key}: build") {
    container.sh """
      cd build
      . ./activate_run.sh
      make all unit_tests > ${container.key}-build.log
    """
    container.copyFrom("build/${container.key}-build.log", "${container.key}-build.log")
    archiveArtifacts "${container.key}-build.log"
  }  // stage

  if (container.key == test_os) {
    pipeline_builder.stage("${container.key}: test") {
      def test_output = "TestResults.xml"
      container.sh """
        cd build
        . ./activate_run.sh
        ./unit_tests/unit_tests --gtest_output=xml:${test_output} --gtest_filter=-'GraylogConnectionCom.IPv6ConnectionTest'
      """
      container.copyFrom('build', '.')
      junit "build/${test_output}"
    }  // stage
  }  // if

  if (container.key == clangformat_os) {
    pipeline_builder.stage("${container.key}: Formatting") {
          if (!env.CHANGE_ID) {
            // Ignore non-PRs
            return
          }
          try {
            // Do clang-format of C++ files
            container.setupLocalGitUser(pipeline_builder.project)
            container.sh """
              clang-format -version
              cd ${project}
              find . \\\\( -name '*.cpp' -or -name '*.cxx' -or -name '*.h' -or -name '*.hpp' \\\\) \\
              -exec clang-format -i {} +
              git status -s
              git add -u
              git commit -m 'GO FORMAT YOURSELF (clang-format)'
            """
          } catch (e) {
           // Okay to fail as there could be no badly formatted files to commit
          } finally {
            // Clean up
          }

          // Push any changes resulting from formatting
          try {
            withCredentials([
              usernamePassword(
              credentialsId: 'cow-bot-username-with-token',
              usernameVariable: 'USERNAME',
              passwordVariable: 'PASSWORD'
              )
            ]) {
              withEnv(["PROJECT=${pipeline_builder.project}"]) {
                container.sh '''
                  cd $PROJECT
                  git push https://$USERNAME:$PASSWORD@github.com/ess-dmsc/$PROJECT.git HEAD:$CHANGE_BRANCH
                '''
              }  // withEnv
            }  // withCredentials
          } catch (e) {
            // Okay to fail; there may be nothing to push
          } finally {
            // Clean up
          }
        }  // stage

    pipeline_builder.stage("${container.key}: cppcheck") {
      def test_output = "cppcheck.xml"
      container.sh """
        cd ${pipeline_builder.project}
        cppcheck --enable=all --inconclusive --template="{file},{line},{severity},{id},{message}" --xml --xml-version=2 src/ 2> ${test_output}
      """
      container.copyFrom("${pipeline_builder.project}/${test_output}", '.')
      recordIssues(tools: [cppCheck(pattern: 'cppcheck.xml')])
    }  // stage
  }  // if
}  // createBuilders

node {
  // Delete workspace when build is done
  cleanWs()

  stage('Checkout') {
    dir("${project}") {
      try {
        scm_vars = checkout scm
      } catch (e) {
        failure_function(e, 'Checkout failed')
      }
    }
  }

  builders['macOS'] = get_macos_pipeline()
  try {
    parallel builders
    } catch (e) {
      pipeline_builder.handleFailureMessages()
      throw e
  }
}

def failure_function(exception_obj, failureMessage) {
    def toEmails = [[$class: 'DevelopersRecipientProvider']]
    emailext body: '${DEFAULT_CONTENT}\n\"' + failureMessage + '\"\n\nCheck console output at $BUILD_URL to view the results.', recipientProviders: toEmails, subject: '${DEFAULT_SUBJECT}'
    throw exception_obj
}

def get_macos_pipeline()
{
    return {
        stage("macOS") {
            node ("macos") {
                // Delete workspace when build is done
                cleanWs()

                dir("${project}/code") {
                    try {
                        checkout scm
                    } catch (e) {
                        failure_function(e, 'MacOSX / Checkout failed')
                    }
                }

                dir("${project}/build") {
                    try {
                        sh "conan install --build=outdated ../code"
                        sh "source activate_run.sh && cmake ../code"
                    } catch (e) {
                        failure_function(e, 'MacOSX / CMake failed')
                    }

                    try {
                        sh "source activate_run.sh && make all unit_tests"
                        sh "source activate_run.sh && ./unit_tests/unit_tests"
                    } catch (e) {
                        failure_function(e, 'MacOSX / build+test failed')
                    }
                }

            }
        }
    }
}
