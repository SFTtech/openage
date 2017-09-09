Seems most structures are only valid as long as their VkInstance exists - how to account for this?
Maybe factor out count; enumerate(&count); vector<>(count); enumerate(vector); into a single generic method
