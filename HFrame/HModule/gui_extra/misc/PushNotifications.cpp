/*
  ==============================================================================

   This file is part of the H library.
   Copyright (c) 2017 - ROLI Ltd.

   H is an open source library subject to commercial or open-source
   licensing.

   By using H, you agree to the terms of both the H 5 End-User License
   Agreement and H 5 Privacy Policy (both updated and effective as of the
   27th April 2017).

   End User License Agreement: www.H.com/H-5-licence
   Privacy Policy: www.H.com/H-5-privacy-policy

   Or: You may also use this code under the terms of the GPL v3 (see
   www.gnu.org/licenses).

   H IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
   EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
   DISCLAIMED.

  ==============================================================================
*/

//==============================================================================
#if ! HANDROID && ! HIOS && ! HMAC
bool PushNotifications::Notification::isValid() const noexcept { return true; }
#endif

PushNotifications::Notification::Notification (const Notification& other)
    : identifier (other.identifier),
      title (other.title),
      body (other.body),
      subtitle (other.subtitle),
      groupId (other.groupId),
      badgeNumber (other.badgeNumber),
      soundToPlay (other.soundToPlay),
      properties (other.properties),
      category (other.category),
      triggerIntervalSec (other.triggerIntervalSec),
      repeat (other.repeat),
      icon (other.icon),
      channelId (other.channelId),
      largeIcon (other.largeIcon),
      tickerText (other.tickerText),
      actions (other.actions),
      progress (other.progress),
      person (other.person),
      type (other.type),
      priority (other.priority),
      lockScreenAppearance (other.lockScreenAppearance),
      publicVersion (other.publicVersion.get() != nullptr ? new Notification (*other.publicVersion) : nullptr),
      groupSortKey (other.groupSortKey),
      groupSummary (other.groupSummary),
      accentColour (other.accentColour),
      ledColour (other.ledColour),
      ledBlinkPattern (other.ledBlinkPattern),
      vibrationPattern (other.vibrationPattern),
      shouldAutoCancel (other.shouldAutoCancel),
      localOnly (other.localOnly),
      ongoing (other.ongoing),
      alertOnlyOnce (other.alertOnlyOnce),
      timestampVisibility (other.timestampVisibility),
      badgeIconType (other.badgeIconType),
      groupAlertBehaviour (other.groupAlertBehaviour),
      timeoutAfterMs (other.timeoutAfterMs)
{
}

//==============================================================================
HIMPLEMENT_SINGLETON (PushNotifications)

PushNotifications::PushNotifications()
  #if HPUSH_NOTIFICATIONS
    : pimpl (new Pimpl (*this))
  #endif
{
}

PushNotifications::~PushNotifications() { clearSingletonInstance(); }

void PushNotifications::addListener (Listener* l)      { listeners.add (l); }
void PushNotifications::removeListener (Listener* l)   { listeners.remove (l); }

void PushNotifications::requestPermissionsWithSettings (const PushNotifications::Settings& settings)
{
  #if HPUSH_NOTIFICATIONS && (HIOS || HMAC)
    pimpl->requestPermissionsWithSettings (settings);
  #else
    ignoreUnused (settings);
    listeners.call ([] (Listener& l) { l.notificationSettingsReceived ({}); });
  #endif
}

void PushNotifications::requestSettingsUsed()
{
  #if HPUSH_NOTIFICATIONS && (HIOS || HMAC)
    pimpl->requestSettingsUsed();
  #else
    listeners.call ([] (Listener& l) { l.notificationSettingsReceived ({}); });
  #endif
}

bool PushNotifications::areNotificationsEnabled() const
{
  #if HPUSH_NOTIFICATIONS
    return pimpl->areNotificationsEnabled();
  #else
    return false;
  #endif
}

void PushNotifications::getDeliveredNotifications() const
{
  #if HPUSH_NOTIFICATIONS
    pimpl->getDeliveredNotifications();
  #endif
}

void PushNotifications::removeAllDeliveredNotifications()
{
  #if HPUSH_NOTIFICATIONS
    pimpl->removeAllDeliveredNotifications();
  #endif
}

String PushNotifications::getDeviceToken() const
{
  #if HPUSH_NOTIFICATIONS
    return pimpl->getDeviceToken();
  #else
    return {};
  #endif
}

void PushNotifications::setupChannels (const Array<ChannelGroup>& groups, const Array<Channel>& channels)
{
  #if HPUSH_NOTIFICATIONS
    pimpl->setupChannels (groups, channels);
  #else
    ignoreUnused (groups, channels);
  #endif
}

void PushNotifications::getPendingLocalNotifications() const
{
  #if HPUSH_NOTIFICATIONS
    pimpl->getPendingLocalNotifications();
  #endif
}

void PushNotifications::removeAllPendingLocalNotifications()
{
  #if HPUSH_NOTIFICATIONS
    pimpl->removeAllPendingLocalNotifications();
  #endif
}

void PushNotifications::subscribeToTopic (const String& topic)
{
  #if HPUSH_NOTIFICATIONS
    pimpl->subscribeToTopic (topic);
  #else
    ignoreUnused (topic);
  #endif
}

void PushNotifications::unsubscribeFromTopic (const String& topic)
{
  #if HPUSH_NOTIFICATIONS
    pimpl->unsubscribeFromTopic (topic);
  #else
    ignoreUnused (topic);
  #endif
}


void PushNotifications::sendLocalNotification (const Notification& n)
{
  #if HPUSH_NOTIFICATIONS
    pimpl->sendLocalNotification (n);
  #else
    ignoreUnused (n);
  #endif
}

void PushNotifications::removeDeliveredNotification (const String& identifier)
{
  #if HPUSH_NOTIFICATIONS
    pimpl->removeDeliveredNotification (identifier);
  #else
    ignoreUnused (identifier);
  #endif
}

void PushNotifications::removePendingLocalNotification (const String& identifier)
{
  #if HPUSH_NOTIFICATIONS
    pimpl->removePendingLocalNotification (identifier);
  #else
    ignoreUnused (identifier);
  #endif
}

void PushNotifications::sendUpstreamMessage (const String& serverSenderId,
                                             const String& collapseKey,
                                             const String& messageId,
                                             const String& messageType,
                                             int timeToLive,
                                             const StringPairArray& additionalData)
{
  #if HPUSH_NOTIFICATIONS
    pimpl->sendUpstreamMessage (serverSenderId,
                                collapseKey,
                                messageId,
                                messageType,
                                timeToLive,
                                additionalData);
  #else
    ignoreUnused (serverSenderId, collapseKey, messageId, messageType);
    ignoreUnused (timeToLive, additionalData);
  #endif
}


