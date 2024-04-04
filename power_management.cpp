#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/battery_state.hpp"
#include "dji_power_management.h"

class PowerManagementNode : public rclcpp::Node
{
public:
    PowerManagementNode() : Node("power_management_node")
    {
        RCLCPP_INFO(rclcpp::get_logger("power_management_node"), "Subscriber started.");
        subscription_ = this->create_subscription<sensor_msgs::msg::BatteryState>(
            "/wrapper/psdk_ros2/battery", 10, std::bind(&PowerManagementNode::batteryCallback, this, std::placeholders::_1));
    }

private:
    //static T_DjiReturnCode DjiTest_PowerOffNotificationCallback(bool *powerOffPreparationFlag);

    void batteryCallback(const sensor_msgs::msg::BatteryState::SharedPtr msg)
    {   
        RCLCPP_INFO(rclcpp::get_logger("power_management_node"), "percentage remaining is '%f'", msg->percentage);
        float batteryPercentage = msg->percentage;
        if (batteryPercentage > 0.15)
        {
            // Register the power off notification callback
            T_DjiReturnCode result = registerPowerOffCallback();

            if (result == DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
            {
                RCLCPP_INFO(rclcpp::get_logger("power_management_node"), "Callback registered successfully");
            }
            else
            {
                RCLCPP_ERROR(rclcpp::get_logger("power_management_node"), "Failed to register callback");
            }
        }
    }

    T_DjiReturnCode registerPowerOffCallback()
    {
        // Register the callback function
        T_DjiReturnCode returnCode = DjiPowerManagement_RegPowerOffNotificationCallback(DjiTest_PowerOffNotificationCallback);
        RCLCPP_INFO(rclcpp::get_logger("power_management_node"), "Callback registered successfully '%ld'", returnCode); 
        // Check if registration was successful
        if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
        {
            RCLCPP_ERROR(rclcpp::get_logger("power_management_node"), "Register power off notification callback function error");
        }
    
        return returnCode;
    }

    // Callback function for power off notification
    T_DjiReturnCode DjiTest_PowerOffNotificationCallback(bool *powerOffPreparationFlag)
    {
        // Perform any necessary actions for shutdown preparation
        // For example, log a message and set the power off preparation flag to true
        RCLCPP_INFO(rclcpp::get_logger("power_management_node"), "Aircraft will power off soon.");
        *powerOffPreparationFlag = true;

        // Return the execution result
        return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
    }

    rclcpp::Subscription<sensor_msgs::msg::BatteryState>::SharedPtr subscription_;
};
int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<PowerManagementNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
