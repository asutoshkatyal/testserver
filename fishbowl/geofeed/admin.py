from django.contrib import admin
from geofeed.models import PointOfInterest
from geofeed.models import Spot
from geofeed.models import Feed
from geofeed.models import Comment
from geofeed.models import testtable
from django.contrib.auth import get_user_model
#from authemail.admin import EmailUserAdmin
#from authemail.admin import EmailUserAdmin
#class MyUserAdmin(EmailUserAdmin):
   # fieldsets = ((None, {'fields': ('email', 'password')}))
admin.site.register(PointOfInterest)
admin.site.register(Spot)
admin.site.register(Feed)
admin.site.register(Comment)
admin.site.register(testtable)
#admin.site.unregister(get_user_model())
#admin.site.register(get_user_model(), MyUserAdmin)