require 'test_helper'

class IotControllerTest < ActionController::TestCase
  test "should get led" do
    get :led
    assert_response :success
  end

end
