from django.contrib.auth.models import User, Group
from rest_framework import serializers
from geofeed.models import testtable
from geofeed.models import Feed,Spot,Comment

class TestTableSerializer(serializers.HyperlinkedModelSerializer):
    #testtableob = serializers.PrimaryKeyRelatedField(many=True)
    class Meta:
        model = testtable
        fields = ('title','description')

class UserSerializer(serializers.HyperlinkedModelSerializer):
    local_feeds = serializers.PrimaryKeyRelatedField(many = True)
    class Meta:
        model = User
        fields = ('url', 'username', 'email', 'local_feeds')


class GroupSerializer(serializers.HyperlinkedModelSerializer):
    class Meta:
        model = Group
        fields = ('url', 'name')


class SpotSerializer(serializers.HyperlinkedModelSerializer):
    #feeds = serializers.HyperlinkedIdentityField(view_name='feed-detail')
    local_feeds = serializers.HyperlinkedRelatedField(many=True, view_name='feed-detail')
   # feeds = serializers.SlugRelatedField(many=True, read_only=True,
    #                                      slug_field='title')
    class Meta:
        model = Spot
        fields = ('name','position','local_feeds')

class FeedSerializer(serializers.HyperlinkedModelSerializer):
    #spots = SpotSerializer(required=True)
    #spots= # serializers.HyperlinkedRelatedField(many=True,view_name='spots-detail')
    # comments = serializers.HyperlinkedIdentityField(view_name='comment-detail')
    comments = serializers.HyperlinkedRelatedField(many=True, view_name='comment-detail')
    class Meta:
        model = Feed
        fields = ('spots','title','description','user', 'comments','pub_date')

class CommentSerializer(serializers.HyperlinkedModelSerializer):
    class Meta:
        model= Comment
        fields =('feed','comment','pub_date','user')