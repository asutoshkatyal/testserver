from django.contrib.auth.models import User, Group
from rest_framework import viewsets
from serializers import UserSerializer, GroupSerializer, TestTableSerializer,FeedSerializer, SpotSerializer,CommentSerializer
from geofeed.models import testtable,Feed,Spot,Comment
from rest_framework import permissions
from permissions import IsOwnerOrReadOnly
from allauth.socialaccount.providers.facebook.views import FacebookOAuth2Adapter
from rest_auth.registration.views import SocialLogin

class FacebookLogin(SocialLogin):
    adapter_class = FacebookOAuth2Adapter

class TestTableSerializerViewSet(viewsets.ModelViewSet):
    queryset = testtable.objects.all()
    serializer_class = TestTableSerializer

class UserViewSet(viewsets.ModelViewSet):
    """
    API endpoint that allows users to be viewed or edited.
    """
    queryset = User.objects.all()
    serializer_class = UserSerializer


class GroupViewSet(viewsets.ModelViewSet):
    """
    API endpoint that allows groups to be viewed or edited.
    """
    queryset = Group.objects.all()
    serializer_class = GroupSerializer

class FeedViewSet(viewsets.ModelViewSet):
    """
    API endpoint that allows groups to be viewed or edited.
    """
   # def pre_save(self, obj):
       # obj.Spot = self.request.Spot
    #permission_classes = (permissions.IsAuthenticatedOrReadOnly,)
    #permission_classes = (permissions.IsAuthenticatedOrReadOnly,
                  #    IsOwnerOrReadOnly,)
    queryset = Feed.objects.all()
    serializer_class = FeedSerializer


class SpotViewSet(viewsets.ModelViewSet):
    """
    API endpoint that allows groups to be viewed or edited.
    """
   # permission_classes = (permissions.IsAuthenticatedOrReadOnly,
               #       IsOwnerOrReadOnly,)
    queryset = Spot.objects.all()
    serializer_class = SpotSerializer

class CommentViewSet(viewsets.ModelViewSet):
    """
    API endpoint that allows groups to be viewed or edited.
    """
   # def pre_save(self, obj):
       # obj.Spot = self.request.Spot
    #permission_classes = (permissions.IsAuthenticatedOrReadOnly,)
    queryset = Comment.objects.all()
    serializer_class = CommentSerializer

