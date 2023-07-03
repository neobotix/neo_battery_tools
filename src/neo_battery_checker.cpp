#include <rclcpp/rclcpp.hpp>
#include <neo_msgs2/msg/relay_board_v2.hpp>

class NeoBatteryChecker : public rclcpp::Node
{
public:
  NeoBatteryChecker() : Node("neo_battery_checker")
  {
    // Declare parameters
    this->declare_parameter<double>("battery_threshold", 20.0);
    this->declare_parameter<bool>("log_battery_voltage", false);

    // Get parameters
    this->get_parameter("battery_threshold", batteryThreshold_);
    this->get_parameter("log_battery_voltage", logBatteryVoltage_);

    // Subscribe to the "state" topic
    stateSubscriber_ = this->create_subscription<neo_msgs2::msg::RelayBoardV2>(
      "state", 10, std::bind(&NeoBatteryChecker::stateCallback, this, std::placeholders::_1));
  }

private:
  void stateCallback(const neo_msgs2::msg::RelayBoardV2::SharedPtr msg)
  {
    // Convert battery percentage to voltage
    float batteryPercentage = msg->battery_voltage * 100;
    float batteryVoltage = msg->battery_voltage;

    if (logBatteryVoltage_) {
      RCLCPP_INFO(this->get_logger(), "Battery percentage: %.2f%%", batteryPercentage);
      RCLCPP_INFO(this->get_logger(), "Battery voltage: %.2fV", batteryVoltage);
    }

    // Check if battery percentage goes below the threshold
    if (batteryPercentage < batteryThreshold_)
    {
      RCLCPP_WARN(this->get_logger(), "Battery percentage is below the user specified threshold: %.2f%%", batteryPercentage);
      
      // Execute the sudo halt command to shut down the computer
      std::system("sudo halt -p");
    }
  }

  double batteryThreshold_;
  bool logBatteryVoltage_;
  rclcpp::Subscription<neo_msgs2::msg::RelayBoardV2>::SharedPtr stateSubscriber_;
};

int main(int argc, char **argv)
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<NeoBatteryChecker>();
  RCLCPP_INFO(node->get_logger(), "Starting the battery checker node");
  rclcpp::spin(node);
  return 0;
}