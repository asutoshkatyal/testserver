from geoposition.fields import GeopositionField
from django.db import models
#from authemail.models import EmailUserManager, EmailAbstractUser


class PointOfInterest(models.Model):
    name = models.CharField(max_length=100)
    position = GeopositionField()
# Create your models here.
class testtable(models.Model):

    title = models.CharField(max_length=100)
    description = models.CharField(max_length=100)

class Spot(models.Model):
    name = models.CharField(max_length=100)
    position = GeopositionField()
    #pub_date =  models.DateTimeField('date published')(null=True)

class Feed(models.Model):
    spots = models.ForeignKey(Spot, related_name='local_feeds')
    title = models.CharField(max_length=100)
    description= models.CharField(max_length=200)
   # pub_date =  models.DateTimeField('date published')(null=True)
    pub_date = models.DateTimeField(null=True)
    user = models.CharField(max_length=50)
    #user = models.ForeignKey(settings.AUTH_USER_MODEL)

class Comment(models.Model):
    feed = models.ForeignKey(Feed,related_name="comments")
    comment = models.CharField(max_length=100)
    pub_date = models.DateTimeField('date published')
    user =  models.CharField(max_length=50)
    #user = models.ForeignKey(settings.AUTH_USER_MODEL)
    """
class MyUser(EmailAbstractUser):
    # Custom fields
    date_of_birth = models.DateField('Date of birth', null=True,
        blank=True)

    # Required
    objects = EmailUserManager()
        """