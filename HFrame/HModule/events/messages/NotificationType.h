

//==============================================================================
/**
    These enums are used in various classes to indicate whether a notification
    event should be sent out.
*/
enum NotificationType
{
    dontSendNotification = 0,   /**< No notification message should be sent. */
    sendNotification = 1,       /**< Requests a notification message, either synchronous or not. */
    sendNotificationSync,       /**< Requests a synchronous notification. */
    sendNotificationAsync,      /**< Requests an asynchronous notification. */
};


