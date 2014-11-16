from django.conf.urls import patterns, include, url
from django.contrib import admin
#from django.shortcuts import redirect
#from django.views.generic import RedirectView
from rest_framework.urlpatterns import format_suffix_patterns
from geofeed import views
from rest_framework import routers
from geofeed import views
from django.conf.urls import include
#from allauth.socialaccount.providers.facebook.views import FacebookOAuth2Adapter
#from rest_auth.registration.views import SocialLogin


router = routers.DefaultRouter()
router.register(r'users', views.UserViewSet)
router.register(r'groups', views.GroupViewSet)
router.register(r'testtable',views.TestTableSerializerViewSet)
router.register(r'Feed',views.FeedViewSet)
router.register(r'Spot',views.SpotViewSet)
router.register(r'Comment',views.CommentViewSet)
urlpatterns = patterns('',
    # Examples:
    # url(r'^$', 'fishbowl.views.home', name='home'),
    # url(r'^blog/', include('blog.urls')),

    url(r'^admin/', include(admin.site.urls)),
    url(r'^oauth2/', include('provider.oauth2.urls', namespace = 'oauth2')),
    #url(r'^accounts/', include('allauth.urls')),
    #url(r'^some-page/$', RedirectView.as_view(pattern_name='my_named_pattern'))
    #url(r'^accounts/profile',redirect('fishbowl/templates/registration/base.html')),
    #url(r'^accounts/', include('registration.backends.default.urls')),
    #/Users/asutoshkatyal/Desktop/fishbowl/fishbowl/templates/registration/index.html
   # (r'^accounts/', include('allauth.urls')),
   # (r'^goscale/', include('goscale.urls')),
    url(r'^', include(router.urls)),
    url(r'^api-auth/', include('rest_framework.urls', namespace='rest_framework')),
    url(r'^accounts/', include('allauth.urls')),
    url(r'^rest-auth/', include('rest_auth.urls')),
    url(r'^rest-auth/registration/', include('rest_auth.registration.urls')),
   # url(r'^/rest-auth/facebook/$', FacebookLogin.as_view(), name='fb_login'),

)
urlpatterns += [
    url(r'^api-auth/', include('rest_framework.urls',
                               namespace='rest_framework')),

]



#urlpatterns = format_suffix_patterns(urlpatterns)