"""
Django settings for fishbowl project.

For more information on this file, see
https://docs.djangoproject.com/en/1.7/topics/settings/

For the full list of settings and their values, see
https://docs.djangoproject.com/en/1.7/ref/settings/
"""

# Build paths inside the project like this: os.path.join(BASE_DIR, ...)
import os
BASE_DIR = os.path.dirname(os.path.dirname(__file__))
#TEMPLATE_DIRS = [os.path.join(BASE_DIR, 'templates')]
SETTINGS_PATH = os.path.dirname(os.path.dirname(__file__))
#TEMPLATE_DIRS = (
  #  os.path.join(SETTINGS_PATH, 'templates'),
#)
TEMPLATE_DIRS = (
    '/Users/asutoshkatyal/Desktop/fishbowl/fishbowl/templates',
)


# Quick-start development settings - unsuitable for production
# See https://docs.djangoproject.com/en/1.7/howto/deployment/checklist/

# SECURITY WARNING: keep the secret key used in production secret!
SECRET_KEY = 'm+3lqe*!6kqdb_$v!eq06b(u)^$z84)%-u(yl!ti@b*hyj=0iu'

# SECURITY WARNING: don't run with debug turned on in production!
DEBUG = True

TEMPLATE_DEBUG = True

ALLOWED_HOSTS = []


# Application definition

INSTALLED_APPS = (
    'django.contrib.admin',
    'django.contrib.auth',
    'django.contrib.contenttypes',
    'django.contrib.sessions',
    'django.contrib.messages',
    'django.contrib.staticfiles',
    "geoposition",
    'geofeed',
    'provider',
    'provider.oauth2',
    'rest_framework',
    #'snippets',
   'django.contrib.sites',
    #'registration',
    'rest_framework.authtoken',
  "rest_auth",
     #'rest_framework.authtoken',
    #'authemail',

    'allauth',
    'allauth.account',
    'rest_auth.registration'
   # 'allauth.socialaccount',
    # ... include the providers you want to enable:
    #'allauth.socialaccount.providers.angellist',
   # 'allauth.socialaccount.providers.bitbucket',
   # 'allauth.socialaccount.providers.bitly',
   # 'allauth.socialaccount.providers.dropbox',
    #'allauth.socialaccount.providers.facebook',
   # 'allauth.socialaccount.providers.flickr',

)
REST_FRAMEWORK = {
    'DEFAULT_PERMISSION_CLASSES': ('rest_framework.permissions.IsAdminUser',),
    #'DEFAULT_AUTHENTICATION_CLASSES': (
     #   'rest_framework.authentication.TokenAuthentication',
    #),
    'PAGINATE_BY': 10
}
#AUTH_USER_MODEL = 'geofeed.MyUser'
AUTH_USER_MODEL = 'auth.User'
#LOGIN_REDIRECT_URL = '/'
#ACCOUNT_AUTHENTICATION_METHOD = 'username_email'
#ACCOUNT_EMAIL_VERIFICATION = 'optional'
ACCOUNT_ACTIVATION_DAYS = 7
SITE_ID = 1
MIDDLEWARE_CLASSES = (
    'django.contrib.sessions.middleware.SessionMiddleware',
    'django.middleware.common.CommonMiddleware',
    'django.middleware.csrf.CsrfViewMiddleware',
    'django.contrib.auth.middleware.AuthenticationMiddleware',
    'django.contrib.auth.middleware.SessionAuthenticationMiddleware',
    'django.contrib.messages.middleware.MessageMiddleware',
    'django.middleware.clickjacking.XFrameOptionsMiddleware',
)

ROOT_URLCONF = 'fishbowl.urls'

WSGI_APPLICATION = 'fishbowl.wsgi.application'


# Database
# https://docs.djangoproject.com/en/1.7/ref/settings/#databases

DATABASES = {
    'default': {
        'ENGINE': 'django.db.backends.sqlite3',
        #'NAME': os.path.join(BASE_DIR, 'db.sqlite3'),
        'NAME': 'tmp.db',
        'USER': '',
        'PASSWORD': '',
        'HOST': '',
        'PORT': '',
    }
}

# Internationalization
# https://docs.djangoproject.com/en/1.7/topics/i18n/

LANGUAGE_CODE = 'en-us'

TIME_ZONE = 'UTC'

USE_I18N = True

USE_L10N = True

USE_TZ = True


# Static files (CSS, JavaScript, Images)
# https://docs.djangoproject.com/en/1.7/howto/static-files/

STATIC_URL = '/static/'
TEMPLATE_LOADERS = (
    'django.template.loaders.filesystem.Loader',
    'django.template.loaders.app_directories.Loader',
)


#TEMPLATE_CONTEXT_PROCESSORS = (
#
#     # Required by allauth template tags
#     "django.core.context_processors.request",
#
#     # allauth specific context processors
#     "allauth.account.context_processors.account",
 #  "allauth.socialaccount.context_processors.socialaccount",
#     "django.contrib.auth.context_processors.auth",
# )


# AUTHENTICATION_BACKENDS = (
#
#     # Needed to login by username in Django admin, regardless of `allauth`
#     "django.contrib.auth.backends.ModelBackend",
#
#     # # `allauth` specific authentication methods, such as login by e-mail
#     # "allauth.account.auth_backends.AuthenticationBackend",
#
# )
