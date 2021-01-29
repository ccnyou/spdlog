#
# Be sure to run `pod lib lint spdlog.podspec' to ensure this is a
# valid spec before submitting.
#
# Any lines starting with a # are optional, but their use is encouraged
# To learn more about a Podspec see https://guides.cocoapods.org/syntax/podspec.html
#

Pod::Spec.new do |s|
  s.name             = 'spdlog'
  s.version          = '1.0.0'
  s.summary          = 'A short description of spdlog.'

# This description is used to generate tags and improve search results.
#   * Think: What does it do? Why did you write it? What is the focus?
#   * Try to keep it short, snappy and to the point.
#   * Write the description between the DESC delimiters below.
#   * Finally, don't worry about the indent, CocoaPods strips it!

  s.homepage         = 'https://github.com/ccnyou/spdlog'
  s.license          = { :type => 'MIT', :file => 'LICENSE' }
  s.author           = { 'ccnyou' => 'ccnyou@qq.com' }
  s.source           = { :git => 'https://github.com/ccnyou/spdlog.git' }
  s.source_files = 'include/**/*'
  s.public_header_files = 'include/*.h'
  s.ios.deployment_target = '9.0'

end
